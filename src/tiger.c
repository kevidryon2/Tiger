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
#include <netinet/ip.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <errno.h>
#include <stdbool.h>
#include <signal.h>
#include <fnmatch.h>
#include "hirolib.h"
#include "bns.h"
#include "server.h"

LoadedScript *scripts;
int nloadedscripts = 1;

const char *verbs[] = {"GET","POST","PUT","PATCH","DELETE","HEAD","OPTIONS"};

const char *httpcodes[] = {
	[200]="OK",
	[204]="No Content",
	[206]="Partial Content",
	[400]="Bad Request",
	[401]="Unauthorized",
	[403]="Forbidden",
	[404]="Not Found",
	[418]="I'm A Teapot",
	[500]="Internal Server Error",
	[501]="Not Implemented",
	[503]="Service Unavailable",
	[505]="HTTP Version Not Supported",
	[507]="Insufficient Storage"
};

char *ntoken(char *const s, char *d, int t);
int startswith(char *s, char *c);
int needle(char *n, char **h, int lh);

bool exists(char *path) {
	FILE *fp = fopen(path, "r");
	if (fp) {
		fclose(fp);
		return true;
	}
	return false;
}

int TigerSearchScript(char *path, int pathlen) {
	for (int i=0; i<nloadedscripts; i++) {
		for (int j=0; j<16; j++) {
			if (scripts[i].paths[j]) {
				if (fnmatch(scripts[i].paths[j], path, 0) != FNM_NOMATCH) {
					return i;
				}
			}
		}
	}
	return -1;
}

RequestData *TigerParseRequest(const char *const reqbuff) {
	RequestData *reqdata = malloc(sizeof(RequestData));
	char *line;
	char *tmp;
	
	if (!reqdata) {
		perror("malloc");
		exit(1);
	}
	
	line = ntoken(reqbuff, "\x0d\x0a", 0);
	
	/* If 2nd token doesn't exist error */
	if (!(tmp = ntoken(line, " ", 2))) {
		errno=1; return NULL;
	}
	
	/* Read request data */
	tmp = ntoken(line, " ", 0);
	strncpy(reqdata->rverb, tmp, 7);
	
	tmp = ntoken(line, " ", 1);
	strncpy(reqdata->path, tmp, 4096);
	
	tmp = ntoken(line, " ", 2);
	strncpy(reqdata->protocol, tmp, 8);
	
	free(line);
	
#ifndef NO_REDIRECT_ROOT
	if (!strcmp(reqdata->path, "/")) {
		strcpy(reqdata->path, "/index.html");
	}
#elif REDIRECT_ROOT_PHP
	if (!strcmp(reqdata->path, "/")) {
		strcpy(reqdata->path, "/index.php");
	}
#endif
	/* Verify client is using HTTP 1.0 or HTTP 1.1 Protocol and using verb GET, POST, PUT, PATCH, DELETE, OPTIONS, or HEAD*/
	if (!(startswith(reqdata->protocol, "HTTP/1.0") ||
		  startswith(reqdata->protocol, "HTTP/1.1"))) {
		errno=2; printf("(%s) (%s) (%s) (%s)", line,
				reqdata->rverb, reqdata->path, reqdata->protocol); return 0;
	} else if ((reqdata->verb = needle(reqdata->rverb, verbs, 7)) < 0) {
		/* Using invalid verb */
		errno=3; return 0;
	}
	if (!reqdata) kill(getpid(), SIGSEGV);
	return reqdata;
}

//Returns a socket fd
int TigerInit(unsigned short port) {
	
	int sock;
	struct sockaddr_in addr = {
		AF_INET,
		htons(port),
		0
	};
	
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	if (sock == -1) {
		perror("socket()");
		exit(127);
	}
	
	if (bind(sock, &addr, sizeof(addr))) {
		perror("bind()");
		exit(127);
	}
	
	int t = 1;
	
	if (listen(sock, 4096)) {
		perror("listen()");
		exit(127);
	}
	
	return sock;
}

loadFile_returnData TigerLoadFile(char *pubpath, char *cachepath) {
	if (!pubpath) {errno=EINVAL; return (loadFile_returnData){0};};
	loadFile_returnData data;

	FILE *pubfile;
	FILE *cachefile;

#ifndef DISABLE_CACHE
	if (!exists(cachepath)) {
#else
	if (true) {
#endif
		//If cached file doesn't exist, cache file
		pubfile = fopen(pubpath, "r");
		cachefile = fopen(cachepath, "w");
		if (!cachefile) {
			printf("E%d %s ", errno, cachepath);
			return (loadFile_returnData){0};
		}

		data.datalen = filesize(pubfile);
		data.data = malloc(data.datalen);
		if (!data.data) {
			perror("malloc");
			exit(1);
		}

		fread(data.data, 1, data.datalen, pubfile);
		fwrite(data.data, 1, data.datalen, cachefile);

		fclose(pubfile);
		fclose(cachefile);
		putchar('N');
	} else {
		cachefile = fopen(cachepath, "r");

		data.datalen = filesize(cachefile);
		data.data = malloc(data.datalen);
		if (!data.data) {
			perror("malloc");
			exit(1);
		}

		fread(data.data, 1, data.datalen, cachefile);
		fclose(cachefile);
		putchar('C');
	}
	return data;
}

void TigerErrorHandler(int status, char *response, RequestData reqdata, char *rootpath) {;
	sprintf(response, "HTTP/1.0 %d %s\nServer: Tiger/"TIGER_VERS"\r\n\r\n", status, httpcodes[status]);
	char *filename = malloc(9); //<status>.html
	sprintf(filename, "%03d.html", status);

	void *tmp1, *tmp2;

	loadFile_returnData data = TigerLoadFile(tmp1 = combine(rootpath, "/public/404.html"), tmp2 = combine(rootpath, "/public/404.html"));
	if (errno) {
		printf("E%d %s ", errno, reqdata.path);
		return (loadFile_returnData){0};
	}

	free(tmp1);
	free(tmp2);
}

int TigerCallPHP(char *source_path, char *output_path, RequestData data, loadFile_returnData *output) {
	char *php_argv_s = "";
	char *php_argv;
	char *command;
	void *tmp;

	//"php ø ø > ø"
	//4+1+3+1 = 9
	//9+strlen(source_path)+strlen(php_argv)+strlen(output_path)

	putchar('P');
	php_argv_s = ntoken(data.path, "?", 1);

	if (!php_argv_s) {
		php_argv = "";
		goto execphp;
	}

	php_argv = malloc(strlen(php_argv_s)+1);
	memset(php_argv, 0, strlen(php_argv_s)+1);


	for (int i=0; i<strlen(php_argv_s); i++) {
		switch (php_argv_s[i]) {
		case '&':
			php_argv[i] = ' ';
			break;
		default:
			php_argv[i] = php_argv_s[i];
		}
	}

execphp:
	command = malloc(9+strlen(source_path)+strlen(php_argv)+strlen(output_path));
	sprintf(command, "php %s %s > %s", source_path, php_argv, output_path);

	if (system(command)) {
		return false;
	}

	printf("S");

	FILE *fp = fopen(output_path, "r");

	char *htmlfile = malloc(filesize(fp));
								   
	if (!htmlfile) {
		perror("malloc");
		exit(255);
	}
	
	fread(htmlfile, 1, filesize(fp), fp);
	output->data = htmlfile;
	output->datalen = filesize(fp);

	fclose(fp);

	free(command);
	return true;
};
