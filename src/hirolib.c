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

void MoveCursorHome() {
	printf("\x1b[H");
}

void MoveCursor(int x, int y) {
	printf("\x1b[%d;%df", y, x);
}

void MoveCursorUp(int n) {
	printf("\x1b[%dA", n);
}

void MoveCursorDown(int n) {
	printf("\x1b[%dB", n);
}

void MoveCursorRight(int n) {
	printf("\x1b[%dC", n);
}

void MoveCursorLeft(int n) {
	printf("\x1b[%dD", n);
}

void MoveCursorUpLine() {
	printf("\x1b M");
}

void SaveCursorPos() {
	printf("\x1b 7");
}

void RestoreCursorPos() {
	printf("\x1b 8");
}

void ClearScreen() {
	printf("\x1b[2J");
}

void ClearLine() {
	printf("\x1b[2K");
}

void EraseLineToCursor() {
	printf("\x1b[1K");
}

void EraseCursorToLine() {
	printf("\x1b[0K");
}

void EraseCursorToScreen() {
	printf("\x1b[0J");
}

void EraseScreenToCursor() {
	printf("\x1b[1J");
}

#define MODE_BOLD 1
#define MODE_DIM 2
#define MODE_ITALIC 3
#define MODE_UNDERLINE 4
#define MODE_BLINKING 5
#define MODE_INVERSE 7
#define MODE_HIDDEN 8
#define MODE_STRIKETROUGH 9

void SetMode(int mode) {	
	printf("\x1b[%dm", mode%10);
}

void ResetMode(int mode) {
	printf("\x1b[%dm", (mode == 1) ? 22 : mode+20);
}

#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_MAGNETA 5
#define COLOR_CYAN 6
#define COLOR_WHITE 7

void ResetColor16() {
	printf("\x1b[0m");
}

void SetColor16(int color) {
	color = color % 16;
	printf("\x1b[%dm", (color<8) ? color+30 : color+82);
}

void SetBgColor16(int color) {
	color = color % 16;
	printf("\x1b[%dm", (color<8) ? color+40 : color+92);
}

void SetColor256(unsigned char color) {
	printf("\x1b[38;5;%dm", color);
}

void SetBgColor256(unsigned char color) {
	printf("\x1b[48;5;%dm", color);
}

void SetFgColorRGB(unsigned char r, unsigned char g, unsigned char b) {
	printf("\x1b[38;2;%d;%d;%dm", r, g, b);
}

void SetBgColorRGB(unsigned char r, unsigned char g, unsigned char b) {
	printf("\x1b[48;2;%d;%d;%dm", r, g, b);
}

#define TEXTMODE_40x25_MONO 0
#define TEXTMODE_40x25_COLOR 1
#define TEXTMODE_80x25_MONO 2
#define TEXTMODE_80x25_COLOR 3
#define GRAPHICSMODE_320x200_2BPP 4
#define GRAPHICSMODE_320x200_1BPP 5
#define GRAPHICSMODE_640x200_1BPP 6
#define LINE_WRAPPING 7
#define GRAPHICSMODE_320x200_COLOR 13
#define GRAPHICSMODE_640x200_4BPP 14
#define GRAPHICSMODE_640x350_1BPP 15
#define GRAPHICSMODE_640x350_4BPP 16
#define GRAPHICSMODE_640x480_1BPP 17
#define GRAPHICSMODE_640x480_4BPP 18
#define GRAPHICSMODE_320x200_8BPP 19

void ScreenSetMode(int mode) {
	printf("\x1b[=%dh", mode);
}

void ScreenResetMode(int mode) {
	printf("\x1b[=%dl", mode);
}

void ResetColor() {
	printf("\x1b[0m");
}
