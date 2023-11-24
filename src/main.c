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
#include "bns.h"
#include "server.h"
#include "c-stacktrace.h"

extern char *verbs[];
extern LoadedScript *scripts;
extern int nloadedscripts;

extern bool disable_cache;
extern bool disable_redirect;
extern bool disable_error;

void sigpipe() {
	printf("(Probably Bogus) ");
}

void segfault() {
	printf("\nTiger crashed!\n\
Please sent all this information in an issue to the Tiger repository \
(https://codeberg.com/kevidryon2/tiger):\n");
	CRITICAL("Recieved signal 11 (SIGSEGV)!");
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


int needle(char *n, char **h, int lh);
char *compile(char *script);
void exec(char *binscript);
int search_begin(char **restrict array, int num_elements, char *restrict string);
int startswith(char *s, char *c);
int endswith(char *restrict s, char *restrict end);
char *combine(char *restrict a, char *restrict b);
char *ntoken(char *const s, char *d, int t);

int TigerInit(unsigned short port);
LoadedScript *TigerLoadScript(char *data, int len);
loadFile_returnData TigerLoadFile(char *pubpath, char *cachepath, int csock);
RequestData *TigerParseRequest(const char *const reqbuff, char *rootpath);
void TigerExecScript(LoadedScript script, RequestData reqdata, char *resbuff);
int TigerSearchScript(char *path, int pathlen);
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

void set_env_variable() {
	printf("You should set the TIGER_PATH environment variable to the path you want to use as the server main directory.\n");
	exit(1);
}

int main(int argc, char **argv, char **envp) {
	char *buffer;
	char scriptpath[PATH_MAX];
	struct dirent *ent;
	FILE *fp;
	int len;
	int sn = 0;
	unsigned short port;
	char *tmp;
	
	printf("Tiger "TIGER_VERS"\n");
	
	init_exceptions(argv[0]);

	signal(SIGPIPE, sigpipe);
	signal(SIGSEGV, segfault);
	
	/* Seed RNG */
	srand(time(NULL));
	
	if (argc<2) {
		port = 8080;
	} else {
		for (int i=1; i<argc; i++) {
			if (argv[i][0] == '-') {
				//options
				for (int j=1; j<strlen(argv[i]); j++) {
					switch (argv[i][j]) {
						case 'n': disable_cache = true; break;
						case 'a': disable_redirect = true; break;
						case 'e': disable_error = true; break;
					}
				}
			} else {
				port = atoi(argv[i]);
			}
		}
	}

	printf("Port: %d\n\n", port);
	
	int serversock = TigerInit(port);
	char rootpath[PATH_MAX];
	char cwdbuffer[PATH_MAX];
	char *fullpath;
	loadFile_returnData read_data;
	
	/* Get server path */
	getcwd(cwdbuffer, PATH_MAX);
	strncpy(rootpath, cwdbuffer, PATH_MAX);
	if (!getenv("TIGER_PATH")) set_env_variable();
	if (!(fullpath = realpath(getenv("TIGER_PATH"), NULL))) {
		perror(getenv("TIGER_PATH"));
		return 127;
	}
	
	strncpy(rootpath, fullpath, PATH_MAX);
	strncat(rootpath, "/", PATH_MAX);
	
	printf("Using directory %s\n", rootpath);
	
	printf("\nLoading scripts...\n");
	
	snprintf(scriptpath, PATH_MAX, "%s/scripts/", rootpath);
	
	DIR *dp = opendir(scriptpath);
	if (!dp) {
		printf("Can't open script directory.\n", errno);
		printf("You should create the 'scripts', 'public', 'cache' (the latter as a ramdisk) directories in the main server folder.\n");
	}
	
	scripts = malloc(sizeof(LoadedScript));
	nloadedscripts=0;
	
	while ((ent = readdir(dp))) {
		if (ent->d_name[0] != '.') {
			if (endswith(ent->d_name, ".bns")) {
				
				/* Open file */
				buffer = combine(scriptpath, ent->d_name);
				fp = fopen(buffer, "r");
				
				if (!fp) {
					perror(buffer);
					return 1;
				}
				
				/* Allocate script */
				nloadedscripts++;
				scripts = realloc(scripts, nloadedscripts*sizeof(LoadedScript));
				
				/* Read file */
				free(buffer);
				fseek(fp, 0, SEEK_END);
				len = ftell(fp);
				buffer = malloc(len);
				
				fseek(fp, 0, SEEK_SET);
				fread(buffer, 1, len, fp);
				
				/* Load file */
				tmp = TigerLoadScript(buffer, len);
				if (!tmp) {
					perror(buffer);
					return 1;
				}
				
				scripts[sn] = *(LoadedScript*)tmp;
				
				sn++;
				fclose(fp);
			}
		}
	};
	closedir(dp);
	
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

		/* Search for a script to handle the request */
		script = TigerSearchScript(reqdata->truepath, strlen(reqdata->truepath));
		if (script > -1) goto script;
		
		/* TODO: Parse headers */
		
		goto noscript;
script:
		printf("(Script) ");
		if (reqdata->verb == VERB_OPTIONS) {
			SetColor16(COLOR_BLUE);
			printf("OPTIONS ");
			ResetColor16();
			/* TODO: Return verbs supported by script */
		}
		TigerExecScript(scripts[script], *reqdata, resbuff);
		goto endreq;
		
noscript:
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
		close(csock);
	}
}
