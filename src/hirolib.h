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

void MoveCursorHome();
void MoveCursor(int x, int y);
void MoveCursorUp(int n);
void MoveCursorDown(int n);
void MoveCursorRight(int n);
void MoveCursorLeft(int n);
void MoveCursorUpLine();
void SaveCursorPos();
void RestoreCursorPos();
void ClearScreen();
void ClearLine();
void EraseLineToCursor();
void EraseCursorToLine();
void EraseCursorToScreen();
void EraseScreenToCursor();

#define MODE_BOLD 1
#define MODE_DIM 2
#define MODE_ITALIC 3
#define MODE_UNDERLINE 4
#define MODE_BLINKING 5
#define MODE_INVERSE 7
#define MODE_HIDDEN 8
#define MODE_STRIKETROUGH 9

void SetMode(int mode);
void ResetMode(int mode);

#define COLOR_BLACK 0
#define COLOR_RED 1
#define COLOR_GREEN 2
#define COLOR_YELLOW 3
#define COLOR_BLUE 4
#define COLOR_MAGNETA 5
#define COLOR_CYAN 6
#define COLOR_WHITE 7

void ResetColor16();
void SetColor16(int color);
void SetBgColor16(int color);
void SetColor256(unsigned char color);
void SetBgColor256(unsigned char color);
void SetFgColorRGB(unsigned char r, unsigned char g, unsigned char b);
void SetBgColorRGB(unsigned char r, unsigned char g, unsigned char b);

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

void ScreenSetMode(int mode);
void ScreenResetMode(int mode);
void ResetColor();
