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
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define max(a,b) ((a)>(b)?(a):(b))
#define min(a,b) ((b)>(a)?(a):(b))

//Search N in H
int needle(char *n, char **h, int lh) {
	for (int i=0; i<lh; i++) {
		if (!strcmp(h[i], n)) {
			return i;
		}
	}
	return -1;
}

char *argparses(int argc, char **argv, int start) {
	char *buffer = malloc(BUFSIZ);
	buffer[0] = 0;
	for (int i=start; i<=argc; i++) {
		strcat(buffer, argv[i]);
		if (i < argc) strcat(buffer, " ");
	}
	return buffer;
}

int search_begin(char **restrict array, int num_elements, char *restrict string) {
	for (int i=0; i<num_elements; i++) {
		if (!strncmp(array[i], string, strlen(string))) {
			return i;
		}
	}
	return -1;
}

int startswith(char *s, char *c) {
	return !strncmp(s, c, min(strlen(c), strlen(s)));
}

int endswith(char *restrict s, char *restrict end) {
	char *so = s+(strlen(s)-strlen(end));
	return !strcmp(so, end);
}

char *combine(char *restrict a, char *restrict b) {
	char *buffer = malloc( strlen(a)+strlen(b)+1 );
	strcpy(buffer, a);
	strcat(buffer, b);
	return buffer;
}

char *ntoken(char *const s, char *d, int t) {
	char *tk = malloc(strlen(s)+1);
	if (!tk) {
		perror("malloc");
	}
	strcpy(tk, s);
	strtok(tk, d);
	for (int i=0; i<t; i++) {
		tk = strtok(0, d);
	}
	return tk;
}

//Count how many times C is in S
int count(char *const s, char c, int l) {
	int count = 0;
	for (int i=0; i<l; i++) {
		count += (s[i] == c);
	}
	
	return count;
}

//parse IP address
uint32_t parse_ip(char *const s) {
	int a, b, c, d;
	if (count(s, '.', strlen(s))) {
		a = strtol(s, &s, 0);
		b = strtol(s+1, &s, 0);
		c = strtol(s+1, &s, 0);
		d = strtol(s+1, &s, 0);
		
		return (uint8_t)(a >> 24) +
		       (uint8_t)(b << 16) +
		       (uint8_t)(c << 8) +
		       (uint8_t)(d << 0);
	} else {
		return strtol(s, NULL, 0);
	}
}
