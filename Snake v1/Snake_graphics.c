/* ============================================================================
*  Snake_graphics.c
* ============================================================================

*  Author:         (c) 2014 Sergio Pedri
*  License:        See the end of this file for license information
*  Created:        July 6, 2014

*  Revision:       2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include "Snake_header.h"

void clear_screen() {
	DWORD n, size;
	COORD coord = {0, 0};
	size = csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
	FillConsoleOutputCharacter(st, TEXT(' '), size, coord, &n);
	GetConsoleScreenBufferInfo(st, &csbiInfo);
	FillConsoleOutputAttribute(st, csbiInfo.wAttributes, size, coord, &n);
	SetConsoleCursorPosition(st, coord);
}

void changeColor(int color) {
	SetConsoleTextAttribute(st, color | color);
}

void restore_colorProfile(int flag) {
	GetConsoleScreenBufferInfo(st, &csbiInfo);
	switch (flag) {
	case 0: colorBackup = csbiInfo.wAttributes; break;
	case 1: SetConsoleTextAttribute(st, colorBackup);
	}
}

void drawBorders(int size) {
	for (int cont = 0; cont < size; cont++) {
		printf("%c", WALL);
	}
	printf("\n");
}

void initializeGraphics() {
	clear_screen();
	restore_colorProfile(0);
	changeColor(GRID_BORDER);
	drawBorders(LENGTH_X);
	for (int cont = 0; cont < LENGTH_Y - 2; cont++) {
		printf("%c", WALL);
		changeColor(BACKGROUND);
		for (int spaces = 0; spaces < LENGTH_X - 2; spaces++) {
			printf(" ");
		}
		changeColor(GRID_BORDER);
		printf("%c\n", WALL);
	}
	drawBorders(LENGTH_X);
	restore_colorProfile(1);
}

void insertTitle() {
	CONSOLE_SCREEN_BUFFER_INFO backupScreenInfo;
	GetConsoleScreenBufferInfo(st, &backupScreenInfo);
	COORD titolo = { INGAME_TITLE_X, INGAME_TITLE_Y };
	SetConsoleCursorPosition(st, titolo);
	restore_colorProfile(0);
	changeColor(GREEN_TEXT);
	printf("Snake "VERSION);
	restore_colorProfile(1);
	COORD tail = { TAIL_TITLE_X, TAIL_TITLE_Y };
	for (int cont = 0; cont < 6; cont++) {
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
		if (cont != 5) {
			tail.X -= 1;
		}
	}
	for (int cont = 0; cont < 2; cont++) {
		tail.Y += 1;
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
	}
	for (int cont = 0; cont < 5; cont++) {
		tail.X -= 1;
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
	}
	for (int cont = 0; cont < 2; cont++) {
		tail.Y += 1;
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
	}
	for (int cont = 0; cont < 7; cont++) {
		tail.X += 1;
		SetConsoleCursorPosition(st, tail);
		if (cont != 6) {
			printf("%c", SQUARE);
		}
		else {
			printf("%c", TRIANGLE_DX);
			printf("  %c", FOOD);
		}
	}
}

void drawSnake(int flag, int** grid) {
	SetConsoleCursorPosition(st, Snake.position);
	restore_colorProfile(0);
	changeColor(SNAKE_COLOR);
	switch (Snake.direction) {
	case UP: printf("%c", TRIANGLE_UP); break;
	case DOWN: printf("%c", TRIANGLE_DOWN); break;
	case RIGHT: printf("%c", TRIANGLE_DX); break;
	case LEFT: printf("%c", TRIANGLE_SX); break;
	}
	fix_overlappedCharacters(Snake.position, grid);
	if (flag) {
		return;
	}
	drawTail(Snake.tail, grid);
	restore_colorProfile(1);
}

void fix_overlappedCharacters(COORD position, int** grid) {
	if (position.X > LENGTH_X - 3) {
		changeColor(DARK_GREEN_TEXT);
		printf("%c", WALL);
		changeColor(SNAKE_COLOR);
		return;
	}
	if (position.X < LENGTH_X - 3) {
		if (grid[position.Y - 1][position.X] == FOOD) {
			changeColor(CYAN_WHITE);
			printf("%c", FOOD);
			changeColor(SNAKE_COLOR);
		}
	}
}

void fix_overlappedTail(int** grid, COORD position, TypeTailSCL tail) {
	if (grid[tail->coordinate.Y - 1][tail->coordinate.X] == 1 &&
		tail->coordinate.X != position.X - 1) {
		COORD temp;
		temp.Y = tail->coordinate.Y;
		temp.X = tail->coordinate.X + 1;
		SetConsoleCursorPosition(st, temp);
		printf("%c", SQUARE);
	}
	if (tail->next != NULL) {
		fix_overlappedTail(grid, position, tail->next);
	}
}

void updateSnakeGraphics(int** grid, int flag) {
	if (!flag) {
		restore_colorProfile(0);
		SetConsoleCursorPosition(st, Snake.backup_tailEnd);
		changeColor(SNAKE_COLOR);
		printf(" ");
		fix_overlappedCharacters(Snake.backup_tailEnd, grid);
		restore_colorProfile(1);
	}
	drawSnake(1, grid);
	SetConsoleCursorPosition(st, Snake.tail->coordinate);
	printf("%c", SQUARE);
	fix_overlappedCharacters(Snake.tail->coordinate, grid);
	restore_colorProfile(1);
}

void updateTailGraphics(TypeTailSCL tailNode, int** grid) {
	SetConsoleCursorPosition(st, tailNode->coordinate);
	printf("%c", SQUARE);
	if (tailNode->next != NULL) {
		updateTailGraphics(tailNode->next, grid);
	}
}

void drawTail(TypeTailSCL tailNode, int** grid) {
	SetConsoleCursorPosition(st, tailNode->coordinate);
	printf("%c", SQUARE);
	fix_overlappedCharacters(tailNode->coordinate, grid);
	if (tailNode->next != NULL) {
		drawTail(tailNode->next, grid);
	}
}

void restoreInGameGraphics(int** grid, int points) {
	initializeGraphics();
	insertTitle();
	drawSnake(0, grid);
	COORD seek_cibo;
	for (int rows = 0; rows < LENGTH_Y - 2; rows++) {
		for (int colonne = 0; colonne < LENGTH_X - 2; colonne++) {
			if (grid[rows][colonne] == FOOD) {
				seek_cibo.Y = rows + 1;
				seek_cibo.X = colonne + 1;
			}
		}
	}
	SetConsoleCursorPosition(st, seek_cibo);
	restore_colorProfile(0);
	changeColor(FOOD_COLOR);
	printf("%c", FOOD);
	restore_colorProfile(1);
	drawInGameBorder();
	displayPauseInstructions();
	updateScore(points);
}

void drawGameOver() {
	COORD gameOver;
	gameOver.X = 0;
	gameOver.Y = LENGTH_Y;
	SetConsoleCursorPosition(st, gameOver);
	restore_colorProfile(0);
	changeColor(RED_TEXT);
	printf(" ______   ______   __    __   ______       ______   __   __ ______   ______\n"
		   "/\\  ___\\ /\\  __ \\ /\\ \"-./  \\ /\\  ___\\     /\\  __ \\ /\\ \\ / //\\  "
		   "___\\ /\\  == \\\n\\ \\ \\__ \\\\ \\  __ \\\\ \\ \\-./\\ \\\\ \\  __\\     \\ "
		   "\\ \\/\\ \\\\ \\ \\'/ \\ \\  __\\ \\ \\  __<\n \\ \\_____\\\\ \\_\\ \\_\\\\ \\"
		   "_\\ \\ \\_\\\\ \\_____\\    \\ \\_____\\\\ \\__|  \\ \\_____\\\\ \\_\\ \\_\\\n"
		   "  \\/_____/ \\/_/\\/_/ \\/_/  \\/_/ \\/_____/     \\/_____/ \\/_/"
		   "    \\/_____/ \\/_/ /_/");
	restore_colorProfile(1);
}

void drawInGameBorder() {
	COORD border;
	border.X = INGAME_TITLE_X - 3;
	border.Y = INGAME_TITLE_Y - 2;
	SetConsoleCursorPosition(st, border);
	restore_colorProfile(0);
	changeColor(DARK_TEXT);
	openGrid(BORDER_LENGTH_X);
	addGridColumns(10, &border, BORDER_LENGTH_X + 1);
	addGridJunctions(&border, BORDER_LENGTH_X);
	addGridColumns(4, &border, BORDER_LENGTH_X + 1);
	addGridJunctions(&border, BORDER_LENGTH_X);
	addGridColumns(3, &border, BORDER_LENGTH_X + 1);
	closeGrid(BORDER_LENGTH_X, &border);
	restore_colorProfile(1);
}

void addGridRows(int width) {
	for (int cont = 0; cont < width; cont++) {
		printf("%c", BAR_WtoE);
	}
}

void addGridJunctions(COORD* border, int width) {
	border->Y += 1;
	SetConsoleCursorPosition(st, *border);
	printf("%c", BAR_N_S_E);
	addGridRows(width);
	printf("%c", BAR_N_S_W);
}

void addGridColumns(int len, COORD* border, int width) {
	for (int rows = 1; rows < len; rows++) {
		border->Y += 1;
		SetConsoleCursorPosition(st, *border);
		printf("%c", BAR_NtoS);
		border->X += width;
		SetConsoleCursorPosition(st, *border);
		printf("%c", BAR_NtoS);
		border->X -= width;
	}
}

void displayPauseInstructions() {
	COORD pausa = { INGAME_TITLE_X - 1, INGAME_TITLE_Y + 13 };
	SetConsoleCursorPosition(st, pausa);
	printf("Press Esc to");
	pausa.Y += 1;
	SetConsoleCursorPosition(st, pausa);
	printf("pause the game");
}

void updateScore(int score) {
	COORD coord_score = { INGAME_TITLE_X, INGAME_TITLE_Y + 10 };
	SetConsoleCursorPosition(st, coord_score);
	restore_colorProfile(0);
	changeColor(DARK_GREEN_TEXT);
	printf("Score: %d", score);
	restore_colorProfile(1);
}

void draw3OptionsGrid(COORD* mainBorder, int levelSelect) {
	changeColor(DARK_TEXT);
	mainBorder -> X = X_POSITION_MENU;
	mainBorder -> Y = Y_POSITION_MENU;
	SetConsoleCursorPosition(st, *mainBorder);
	openGrid(MENU_WIDTH);
	for (int cont = 0; cont < 3; cont++) {
		if (record != 0 && cont == 0 && !levelSelect) {
			addGridColumns(5, mainBorder, MENU_WIDTH + 1);
		}
		else {
			addGridColumns(4, mainBorder, MENU_WIDTH + 1);
		}
		if (cont != 2) {
			addGridJunctions(mainBorder, MENU_WIDTH);
		}
	}
	closeGrid(MENU_WIDTH, mainBorder);
	changeColor(WHITE_TEXT);
	mainBorder -> Y = Y_POSITION_MENU + 2;
	mainBorder -> X = X_POSITION_MENU + 3;
}

void mainMenuGraphics() {
	restore_colorProfile(0);
	changeColor(GREEN_TEXT);
	printf("\n\t ________  ________   ________  ___  __    _______\n"
		   "\t|\\   ____\\|\\   ___  \\|\\   __  \\|\\  \\|\\  \\ |\\  ___ \\ \n"
		   "\t\\ \\  \\___|\\ \\  \\\\ \\  \\ \\  \\|\\  \\ \\  \\/  /|\\ \\   __/|\n"
		   "\t \\ \\_____  \\ \\  \\\\ \\  \\ \\   __  \\ \\   ___  \\ \\  \\_|/__\n"
		   "\t  \\|____|\\  \\ \\  \\\\ \\  \\ \\  \\ \\  \\ \\  \\\\ \\  \\ \\  \\_|\\ \\\n"
		   "\t    ____\\_\\  \\ \\__\\\\ \\__\\ \\__\\ \\__\\ \\__\\\\ \\__\\ \\_______\\\n"
		   "\t   |\\_________\\|__| \\|__|\\|__|\\|__|\\|__| \\|__|\\|________|\n"
		   "\t   \\|_________|\t\t\t\t\t"VERSION);
	COORD mainBorder;
	draw3OptionsGrid(&mainBorder, 0);
	writeOptions("1 ---> New game", "2 ---> Instructions", "0 ---> Exit", 1, &mainBorder);
}

void writeOptions(char* option1, char* option2, char* option3, int includeRecord, COORD* border) {
	for (int cont = 0; cont < 3; cont++) {
		SetConsoleCursorPosition(st, *border);
		switch (cont) {
		case 0: {
			printf(option1);
			if (includeRecord) {
				if (record != 0) {
					border->Y += 1;
					SetConsoleCursorPosition(st, *border);
					changeColor(DARK_GREEN_TEXT);
					printf("   Record: %d points", record);
					changeColor(WHITE_TEXT);
				}
			}
			break;
		}
		case 1: printf(option2); break;
		case 2: printf(option3); break;
		}
		border -> Y += 4;
	}
	restore_colorProfile(1);
}

void clearArea(int x, int y) {
	DWORD n, size;
	COORD cancella_menu = { x, y };
	size = csbiInfo.dwSize.X * 15;
	FillConsoleOutputCharacter(st, TEXT(' '), size, cancella_menu, &n);
	GetConsoleScreenBufferInfo(st, &csbiInfo);
	FillConsoleOutputAttribute(st, csbiInfo.wAttributes, size, cancella_menu, &n);
}

int graphics_levelSelect() {
	clearArea(0, 9);
	COORD position;
	draw3OptionsGrid(&position, 1);
	writeOptions("1 ---> Easy", "2 ---> Normal", "3 ---> Hard", 0, &position);
	int actualSelection;
	while (1) {
		actualSelection = _getch();
		if ((actualSelection == 49) || (actualSelection == 50) || (actualSelection == 51)) {
			switch (actualSelection) {
			case 49: return 100;
			case 50: return 80;
			case 51: return 45;
			}
		}
	}
}

void graphics_SelectGameMode() {
	clearArea(0, 9);
	changeColor(DARK_TEXT);
	COORD switch_mode;
	switch_mode.X = X_POSITION_MENU;
	switch_mode.Y = Y_POSITION_MENU + 1;
	SetConsoleCursorPosition(st, switch_mode);
	openGrid(MENU_WIDTH);
	for (int cont = 0; cont < 2; cont++) {
		addGridColumns(4, &switch_mode, MENU_WIDTH + 1);
		if (cont == 0) {
			addGridJunctions(&switch_mode, MENU_WIDTH);
		}
	}
	closeGrid(MENU_WIDTH, &switch_mode);
	changeColor(WHITE_TEXT);
	switch_mode.Y = Y_POSITION_MENU + 3;
	switch_mode.X = X_POSITION_MENU + 3;
	for (int cont = 0; cont < 2; cont++) {
		SetConsoleCursorPosition(st, switch_mode);
		switch (cont) {
		case 0: printf("1 ---> Classic"); break;
		case 1: printf("2 ---> Snake II"); break;
		}
		switch_mode.Y += 4;
	}
}

void openGrid(int length) {
	printf("%c", BAR_StoE);
	addGridRows(length);
	printf("%c", BAR_StoW);
}

void closeGrid(int width, COORD* position) {
	position->Y += 1;
	SetConsoleCursorPosition(st, *position);
	printf("%c", BAR_NtoE);
	addGridRows(width);
	printf("%c", BAR_WtoN);
}

void createEmptyGrid(int length, COORD* start, int height) {
	SetConsoleCursorPosition(st, *start);
	openGrid(length);
	addGridColumns(height, start, length + 1);
	closeGrid(length, start);
}

void instructions() {
	clear_screen();
	COORD border_instructions;
	setMenu(1, &border_instructions, INSTRUCTIONS_WIDTH, 12);
	for (int cont = 0; cont < 6; cont++) {
		SetConsoleCursorPosition(st, border_instructions);
		switch (cont) {
		case 0: printf("Use the arrows to move the Snake. Every time the Snake"); break;
		case 1: printf("eats some food, it becomes longer."); border_instructions.Y++; break;
		case 2: printf("In classic mode, the game ends when you crash into the wall"); break;
		case 3: printf("or when you touch the tail of the Snake."); border_instructions.Y++; break;
		case 4: {
			printf("In Snake II mode, you can pass through the wall.\n");
			border_instructions.Y += 1;
			changeColor(DARK_TEXT);
			break;
		}
		case 5: printf("Press any key to continue...");
		}
		border_instructions.Y += 1;
	}
	restore_colorProfile(1);
	_getch();
	clear_screen();
}

void setMenu(int flag, COORD* position, int width, int height) {
	restore_colorProfile(0);
	changeColor(GREEN_TEXT);
	if (flag) {
		printf("\n   Snake - Instructions");
	}
	else {
		printf("\n   Snake - Pause");
	}
	changeColor(DARK_TEXT);
	position->X = 2;
	position->Y = 3;
	createEmptyGrid(width, position, height);
	changeColor(WHITE_TEXT);
	position->X = 4;
	position->Y = 5;
}

void pauseGraphics() {
	COORD menu_pausa;
	setMenu(0, &menu_pausa, PAUSE_MENU_WIDTH, 7);
	for (int cont = 0; cont < 3; cont++) {
		SetConsoleCursorPosition(st, menu_pausa);
		switch (cont) {
		case 0: {
			printf("Are you sure yuu want to exit? :(");
			menu_pausa.Y += 1;
			break;
		}
		case 1: printf("Press 1 to keep playing"); break;
		case 2: printf("Press Esc again to go back to the main Menu"); break;
		}
		menu_pausa.Y += 1;
	}
}

/* Copyright (C) 2014 Sergio Pedri

* This library is free software; you can redistribute it and/or
* modify it under the terms of the GNU Lesser General Public
* License as published by the Free Software Foundation; either
* version 2.1 of the License, or (at your option) any later version.

* This library is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
* Lesser General Public License for more details.

* You should have received a copy of the GNU Lesser General Public
* License along with this library; If not, see http://www.gnu.org/licenses/
*/