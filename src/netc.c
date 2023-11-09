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

#include <stdio.h>
#include <string.h>

char *compile(char *code, int *len);

int main(int argc, char **argv) {
	if (argc<3) {
		printf("Usage: %s <file> <output>\n", argv[0]);
		return 1;
	}
	FILE *ifp = fopen(argv[1], "r");
	FILE *ofp = fopen(argv[2], "w");
	
	if (!ifp) {
		perror(argv[1]);
		return 1;
	} else if (!ofp) {
		perror(argv[2]);
		return 1;
	}
	
	fseek(ifp, 0, SEEK_END);
	char *buffer = malloc(ftell(ifp));
	fseek(ifp, 0, SEEK_SET);
	fread(buffer, 1, -1, ifp);
	
	int len;
	char *cbuffer = compile(buffer, &len);
	fwrite(cbuffer, 1, len, ofp);
	
	return 0;
}
