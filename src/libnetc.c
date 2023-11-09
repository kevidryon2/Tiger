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

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "bns.h"

#define NUM_KEYWORDS 4
const char *keywords[NUM_KEYWORDS] = {
	"path", "return", "end", "supports-verb"
};

char *compile(char *code, int *len) {
	int linecount = 1;
	char **lines = calloc(sizeof(char*), 16384);
	char **tokens;
	
	//Split CODE into LINES
	lines[0] = strtok(code, "\n");
	for (int i=1; (lines[i] = strtok(NULL, "\n")); i++, linecount++);

	//Build header
	NSHeader h;
	memcpy(h.magic, "nets", 4);
	h.ver_major = NETC_VERSION_MAJOR;
	h.ver_minor = NETC_VERSION_MINOR;
	memset(h.paths, 0, 64);
	int tokencount;
	
	FILE *scriptfp = tmpfile();
	
	int empty = 0;
	
	for (int i=0; i<linecount; i++) {
		if (!lines[i][0]) continue;
		
		//Find first non-whitespace character of line
		while (lines[i][0] == ' '  || 
			   lines[i][0] == '\t') {
				   lines[i]++;
				   if (!lines[i][0]) empty = 1;
			   }
		
		//If empty string skip
		if (empty) {
			empty = 0;
			goto end; //Yes, I use goto.
		}
		
		//Allocate tokens
		tokens = calloc(sizeof(char*), 16384);
		tokencount = 0;
		
		//Split current line into tokens
		tokens[0] = strtok(lines[i], " ");
		for (int j=1; (tokens[j] = strtok(NULL, " ")); j++, tokencount++);
		
		NSInstruction inst;
		
		//Do the appropriate action for command
		switch (needle(tokens[0], keywords, NUM_KEYWORDS)) {
			case -1:
				printf("Error: Unknown keyword %s at line %d\n", tokens[0], i+1);
				exit(1);
				break;
			
			case 0:
				if (strlen(h.paths)>(64-strlen(h.paths))) {
					printf("Error: Path buffer is full\n");
				}
				printf("Adding path %s\n", argparses(tokencount, tokens, 1));
				strcpy(h.paths+strlen(h.paths)+1, argparses(tokencount, tokens, 1));
				break;
			
			case 1:
				inst.inst_id = INST_RETURN;
				*(unsigned short*)inst.data = atoi(tokens[1]);
				fwrite(&inst, sizeof(NSInstruction), 1, scriptfp);
				
				printf("Added RETURN packet ( Hexdump: ");
				for (int j=0; j<sizeof(NSInstruction); j++) printf("%02x ", ((unsigned char*)&inst)[j]);
				printf(")\n");
				
				inst.inst_id = INST_STRING_PACKET;
				*(unsigned short*)inst.data = strlen(argparses(tokencount, tokens, 2))+1;
				fwrite(&inst, 1, 1, scriptfp);
				fputs(argparses(tokencount, tokens, 2), scriptfp);
				putc(0, scriptfp);
				
				printf("Added STRING packet\n");
				break;
			
			case 2:
				goto stop;
			
			case 3:
				
				break;
		}
		
		end:
		free(tokens);
	}
	
	stop:
	free(lines);
	
	printf("Saving file...\n");
	
	//Allocate
	int oflen = ftell(scriptfp);
	char *scriptbuffer = malloc(oflen+sizeof(NSHeader));
	
	//Read header
	memcpy(scriptbuffer, &h, sizeof(NSHeader));
	
	//Read script
	fseek(scriptfp, 0, SEEK_SET);
	fread(scriptbuffer+sizeof(NSHeader), 1, oflen, scriptfp);
	
	*len = oflen+sizeof(NSHeader);
	
	fclose(scriptfp);
	
	return scriptbuffer;
}
