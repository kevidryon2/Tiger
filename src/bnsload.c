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

#include "bns.h"
#include <errno.h>
#include <string.h>
#include "server.h"

LoadedScript *TigerLoadScript(char *data, int len) {
	
	//If not enough bytes are contained in the file error out
	if (len < sizeof(NSHeader))
		goto notscript;
	
	//Allocate memory
	NSHeader h;
	LoadedScript *s = malloc(sizeof(LoadedScript));
	
	//Read header
	memcpy(&h, data, sizeof(h));
	
	//Check header magic
	if (strncmp(h.magic, "nets", 4))
		goto notscript;
	
	//Check header version
	if (NETC_VERSION_MAJOR != h.ver_major || NETC_VERSION_MINOR != h.ver_minor) {
		printf("(warning: bad file version) ");
	}
	
	char *p = h.paths;
	
	//Load paths
	for (int i=0; p<h.paths+64; i++) {
		s->paths[i] = malloc(strlen(p)+1);
		if (strlen(p)) {
			memcpy(s->paths[i], p, strlen(p)+1);
		}
		p += strlen(p)+1;
	};
	
	memcpy(s->script, data+sizeof(h), len-sizeof(h));
	
	return s;
	
notscript:
	errno=EINVAL;
	return NULL;
}

void TigerExecScript(LoadedScript scripts, RequestData reqdata, char *resbuff) {
	
}
