# Tiger, a web server built for being really fast and powerful.
# Copyright (C) 2023 kevidryon2
# 
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
# 
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.

ARCH=x86_64

all: build/tiger-$(ARCH) 
dynamic: build/tiger-$(ARCH)_dynamic

clean:
	rm -rf build/*

OBJS= \
		build/main.o \
		 build/librsl.o \
		 build/tiger.o \
		 build/hirolib.o \
		 build/daemon.o

CCFLAGS=-pedantic -Wall -O0 -rdynamic
CC=$(ARCH)-linux-gnu-gcc

build/tiger-$(ARCH)_dynamic: $(OBJS)
	$(CC) -g3 $(CFLAGS) $(OBJS) -o build/tiger-$(ARCH)_dynamic $(CCFLAGS)
	
build/tiger-$(ARCH): $(OBJS)
	$(CC) -g3 $(CFLAGS) $(OBJS) -o build/tiger-$(ARCH) $(CCFLAGS) -static

build/%.o: src/%.c
	$(CC) -g3 $(CFLAGS) -c -o $@ $<

install:
	install build/tiger-$(ARCH) /usr/local/bin/tiger
