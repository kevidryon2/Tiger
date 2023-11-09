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

#include <stdlib.h>
#include <stdio.h>
#pragma once

typedef struct {
  char *paths[16];
  int supp_methods[8];
  char script[BUFSIZ];
} LoadedScript;

typedef struct {
	char magic[4]; //Always 'nets'
	unsigned char ver_major;
	unsigned char ver_minor;
  unsigned char supp_methods[8];
	char paths[64]; //An array of zero-terminated path strings
  int ninst;
} NSHeader;

typedef enum {
	INST_NOP 			= 0x30,
	INST_RETURN 		= 0xd6,
	INST_STRING_PACKET	= 0x1e //Followed by a zero-terminated string
} NSInstID;

typedef struct {
	unsigned char inst_id;
	char data[3];
} NSInstruction;

#define NETC_VERSION_MAJOR 0x00
#define NETC_VERSION_MINOR 0x05
