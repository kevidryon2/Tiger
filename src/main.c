/*
 Tiger, a web server built for being really fast and powerful.
 Copyright (C) 2023 kevidryon2
 
 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU Affero General Public License as
 published by the Free Software Foundation, either version 3 of the
 License, or (at your option) any later version.
 
 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU Affero General Public License for more details.
 
 You should have received a copy of the GNU Affero General Public License
 along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include "hirolib.h"
#include "server.h"
#include "librsl.h"
#include "c-stacktrace.h"

extern char *verbs[];

extern bool disable_cache;
extern bool disable_redirect;
extern bool disable_error;
extern uint32_t ip_whitelist;
extern uint32_t ip_mask;

bool create_daemon = false;

void sigpipe() {
	printf("(Probably Bogus) ");
}

int filesize(FILE *fp) {
	int os = ftell(fp);
	fseek(fp, 0, SEEK_END);
	int s = ftell(fp);
	fseek(fp, os, SEEK_SET);
	return s;
}

void logdata(char *data) {
	for (int i=0; i<strlen(data); i++) {
		if (data[i] < ' ' || data[i] > '~') {
			printf("\\%sx%02x", "", (unsigned char)(data[i]));
		} else putchar(data[i]);
	}
}

int TigerInit(unsigned short port);
loadFile_returnData TigerLoadFile(char *pubpath, char *cachepath, int csock);
RequestData *TigerParseRequest(const char *const reqbuff, char *rootpath);
int TigerCallPHP(char *source_path, char *output_path, RequestData data, loadFile_returnData *output);

char *escapestr(unsigned char *s) {
	unsigned char *o = malloc(BUFSIZ);

	memset(o, 0, BUFSIZ);
	
	int j;

	int oi = 0;
	for (int i=0; i<strlen(s); i++) {
		for (j=i; s[j] &&
			 	  s[j] != '/' &&
			 	  s[j] < 0x80 &&
			 	  s[j] > 0x1f; j++);
		j -= i;
		if (j > 0) {
			if (j > 25) j = 25;
			o[oi] = 'A'+j; oi++;
			memcpy(o+oi, s+i, j); oi += j, i += j;
			i--;
		} else {
			o[oi] = (s[i]>>4)+'a'; oi++;
			o[oi] = (s[i]%16)+'a'; oi++;
		}
	}
	
	return o;
}

void usage(char *name) {
	printf("Usage: %s [OPTIONS]\n", name);
	printf("  -p [port]            (required) port to listen to\n");
	printf("  -i [ip]              only allow [ip] to connect\n");
	printf("  -m [ip]              apply [ip] as a mask to the client ips\n");
	printf("  -c [directory]       set working directory to [directory]\n");
	printf("  -d start             start Tiger as daemon\n");
	printf("  -d stop              stop Tiger daemon\n");
	printf("  -d restart           restart Tiger daemon\n");
	printf("  -a                   disable redirecting / to /index.html or /index.php\n");
	printf("  -n                   disable cache\n");
	printf("  -e                   disable using error pages (e.g. /404.html)\n");
	printf("\n");
	printf("An IP address can be specified in one of the following ways:\n");
	printf("    127.0.0.1\n");
	printf("    0xde.0xad.0xbe.0xef\n");
	printf("    12345\n");
	printf("    0xdeadbeef\n");
}

int main(int argc, char **argv) {
	char *buffer;
	char scriptpath[PATH_MAX];
	struct dirent *ent;
	FILE *fp;
	int len;
	int sn = 0;
	unsigned short port;
	char *tmp;
	
	char *fullpath = calloc(1, 64);
	fullpath = getcwd(fullpath, 64);
	
	printf("Tiger "TIGER_VERS"\n");

	signal(SIGPIPE, sigpipe);
	
	/* Seed RNG */
	srand(time(NULL));
	
	char *s;
	int a, b, c, d;
	
	port = -1;
	
	for (int i=1; i<argc; i++) {
		if (argv[i][0] == '-') {
			//options
			for (int j=1; j<strlen(argv[i]); j++) {
				switch (argv[i][j]) {
					case 'n': disable_cache = true; break;
					case 'a': disable_redirect = true; break;
					case 'e': disable_error = true; break;
					case 'i': //ip whitelist
						i++;
						if (!(i < argc)) {
							usage(argv[0]);
							exit(1);
						}
						s = argv[i];
						
						ip_whitelist = parse_ip(s);
						a = (uint8_t)(ip_whitelist >> 24);
						b = (uint8_t)(ip_whitelist >> 16);
						c = (uint8_t)(ip_whitelist >> 8);
						d = (uint8_t)(ip_whitelist >> 0);
						printf("IP Whitelist: %d.%d.%d.%d (%08x)\n", a, b, c, d, ip_whitelist);
						goto skip_arg;
					case 'm': //ip mask
						i++;
						if (!(i < argc)) {
							usage(argv[0]);
							exit(1);
						}
						s = argv[i];
						
						ip_mask = parse_ip(s);
						a = (uint8_t)(ip_mask >> 24);
						b = (uint8_t)(ip_mask >> 16);
						c = (uint8_t)(ip_mask >> 8);
						d = (uint8_t)(ip_mask >> 0);
						printf("IP Mask: %d.%d.%d.%d (%08x)\n", a, b, c, d, ip_mask);
						goto skip_arg;
					case 'p': //port
						i++;
						if (!(i < argc)) {
							usage(argv[0]);
							exit(1);
						}
						s = argv[i];
						port = strtol(s, NULL, 0);
						goto skip_arg;
					case 'c': //change dir
						i++;
						if (!(i < argc)) {
							usage(argv[0]);
							exit(1);
						}
						s = realpath(argv[i], NULL);
						if (!s) {
							perror(argv[i]);
							return 1;
						}
						fullpath = s;
						chdir(s);
						goto skip_arg;
					case 'd': //daemon
						i++;
						if (!(i < argc)) {
							usage(argv[0]);
							exit(1);
						}
						
						//check if running as root
						if (getuid() != 0) {//root has pid 0
							printf("Error: root priviliges required.\n");
							return 1;
						}
						
						if (!strcmp(argv[i], "stop")  | !strcmp(argv[i], "restart")) daemon_stop();
						if (!strcmp(argv[i], "start") | !strcmp(argv[i], "restart")) daemon_start();
						if (!strcmp(argv[i], "stop")) return 0;
						goto skip_arg;
				}
			}
	skip_arg:;
		}
	}
	
	if (port == 65535) {
		usage(argv[0]);
		exit(1);
	}
	
	printf("Port: %d\n\n", port);
	
	if (create_daemon) daemon_init();
	
	int serversock = TigerInit(port);
	
	char rootpath[PATH_MAX];
	char cwdbuffer[PATH_MAX];
	loadFile_returnData read_data;
	
	/* Get server path */
	getcwd(cwdbuffer, PATH_MAX);
	strncpy(rootpath, cwdbuffer, PATH_MAX);
	
	strncpy(rootpath, fullpath, PATH_MAX);
	strncat(rootpath, "/", PATH_MAX);
	
	printf("Using directory %s\n", rootpath);
	
	int csock;
	int statcode;
	int script;
	int bodylen;
	
	struct sockaddr_in caddr;
	socklen_t caddrl;
	char reqbuff[BUFSIZ];
	char *resbuff = malloc(BUFSIZ);
	
	if (!resbuff) {
		perror("malloc");
		return -1;
	}
	
	RequestData *reqdata;
	
	FILE *tmpfp;
	
	FILE *publicfp;
	FILE *cachedfp;
	
	char public_path[PATH_MAX];
	char cached_path[PATH_MAX];
	char phpoutput_path[PATH_MAX];
	char *line;
	char *body;
	
	while (true) {
		/* Accept */
		csock = accept(serversock, &caddr, &caddrl);
		
		if (ip_whitelist & caddr.sin_addr.s_addr) {
			if (ntohl(caddr.sin_addr.s_addr) != ip_whitelist) goto block_req;
		}
		
		printf("%d.%d.%d.%d ",
			   caddr.sin_addr.s_addr%256,
			   (caddr.sin_addr.s_addr>>8)%256,
			   (caddr.sin_addr.s_addr>>16)%256,
			   (caddr.sin_addr.s_addr>>24)%256
		);
		
		/* Clear buffers */
		memset(resbuff, 0, BUFSIZ);
		memset(reqbuff, 0, BUFSIZ);
		memset(&reqdata, 0, sizeof(reqdata));
		
		/* Read request */
		read(csock, reqbuff, BUFSIZ);
		
		/* Parse request */
		
		if (!(reqdata = TigerParseRequest(reqbuff, rootpath))) {
			switch (errno) {
				//using HTTP/0.9
				case 1:
				case 2:
					SetColor16(COLOR_RED);
					printf("HTTP/0.9 ");
					ResetColor16();
					goto endreq;
				
				//Invalid verb
				case 3:
					SetColor16(COLOR_RED);
					printf("Invalid Verb ");
					ResetColor16();
					
					TigerErrorHandler(501, resbuff, reqdata);
					write(csock, resbuff, strlen(resbuff));
					goto endreq;
			}
		}
		
		//printf("%s\n", reqdata->path);
		
		reqdata->truepath = ntoken(reqdata->path, "?", 0);

		/* TODO: Parse headers */
		
		/* If verb is OPTIONS return allowed options (GET, OPTIONS, HEAD) */
		if (reqdata->verb == VERB_OPTIONS) {
			SetColor16(COLOR_BLUE);
			printf("OPTIONS");
			ResetColor16();
			snprintf(resbuff, BUFSIZ, "HTTP/1.0 200 OK\r\nServer: Tiger/"TIGER_VERS"\r\nAllow: OPTIONS, GET, HEAD\r\n");
			write(csock, resbuff, strlen(resbuff));
			goto endreq;
			
		}
		
		/* Fetch file */
		memset(public_path, 0, PATH_MAX);
		snprintf(public_path, sizeof public_path, "%s/public/%s", rootpath, reqdata->truepath);
		
		/* If file doesn't exist in public directory return 404 Not Found */
		if (!(publicfp = fopen(public_path, "r"))) {
			if (errno == 2) {
				SetColor16(COLOR_RED);
				printf("%s ", reqdata->truepath);
				ResetColor16();
				TigerErrorHandler(404, resbuff, *reqdata, rootpath);
			} else {
				SetColor16(COLOR_RED);
				ResetColor16();
				printf("ERROR %d ", errno);
				TigerErrorHandler(500, resbuff, *reqdata, rootpath);
			}
			write(csock, resbuff, strlen(resbuff));
			goto endreq;
		}
		
		fclose(publicfp);
		
		/* File exists */
		//printf("%s ", reqdata->truepath);
		memset(cached_path, 0, PATH_MAX);
		snprintf(cached_path, sizeof cached_path, "%s/cache/%s", rootpath, tmp = escapestr(reqdata->truepath));
		free(tmp);
		snprintf(phpoutput_path, sizeof cached_path, "%s/cache/%s.html", rootpath, tmp = escapestr(reqdata->truepath));

		//printf("%s %s\n", cached_path, public_path);

		read_data = TigerLoadFile(public_path, cached_path, csock);
		
		//printf("'%s' %d %d\n", read_data.data, read_data.datalen, read_data.type);
		
		if (endswith(reqdata->truepath, ".php")) {
			free(read_data.data);
			int ret = TigerCallPHP(disable_cache?cached_path:public_path, phpoutput_path, *reqdata, &read_data);
			if (ret) {
				goto response;
			} else {
				TigerErrorHandler(500, resbuff, *reqdata, rootpath);
				goto endreq;
			}
		}

response:
		/* Send response */
		
		sprintf(resbuff,"HTTP/1.0 200 OK\r\nServer: Tiger/"TIGER_VERS"\r\n\r\n");
		write(csock, resbuff, strlen(resbuff));
		
		/* Finish and flush */
		fflush(stdout);

		write(csock, read_data.data, read_data.datalen);
		free(read_data.data);
		free(reqdata->truepath);
		free(reqdata);
		
endreq:
		/* Finish and flush */
		putchar('\n');
		fflush(stdout);
		
block_req:
		close(csock);
	}
}
