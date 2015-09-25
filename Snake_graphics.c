/* ============================================================================
*  Snake_graphics.c
* ============================================================================

*  Author:         (c) 2014 Sergio Pedri
*  License:        See the end of this file for license information
*  Created:        July 6, 2014
*  Last edit:      November 2014

*  Revision:       2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <conio.h>
#include <Windows.h>
#include "Snake_header.h"

// ---------------------------------------------------------------------
// clear_screen
// ---------------------------------------------------------------------
// Fills the console with blank spaces to clear its content
void clear_screen()
{
	DWORD n, size;
	COORD coord = { 0, 0 };
	size = csbiInfo.dwSize.X * csbiInfo.dwSize.Y;
	FillConsoleOutputCharacter(st, TEXT(' '), size, coord, &n);
	GetConsoleScreenBufferInfo(st, &csbiInfo);
	FillConsoleOutputAttribute(st, csbiInfo.wAttributes, size, coord, &n);
	SetConsoleCursorPosition(st, coord);
}

// ---------------------------------------------------------------------
// changeColor
// ---------------------------------------------------------------------
// Sets a new color combination for each character foreground/background
void changeColor(int color)
{
	SetConsoleTextAttribute(st, color | color);
}

// ---------------------------------------------------------------------
// restore_colorProfile
// ---------------------------------------------------------------------
// Restore the previous console color profile or creates a new backup for later use
void restore_colorProfile(int flag)
{
	GetConsoleScreenBufferInfo(st, &csbiInfo);
	switch (flag)
	{
	case 0: colorBackup = csbiInfo.wAttributes; break;
	case 1: SetConsoleTextAttribute(st, colorBackup);
	}
}

// ---------------------------------------------------------------------
// drawBorders
// ---------------------------------------------------------------------
// Draws a line with the wall character (used for the upper/lower margin of the game field)
void drawBorders(int size)
{
	for (int cont = 0; cont < size; cont++)
	{
		printf("%c", WALL);
	}
	printf("\n");
}

// ---------------------------------------------------------------------
// initializeGraphics
// ---------------------------------------------------------------------
// Clears the screen, restores the color profile, draws the game walls and
// colors the game field background
void initializeGraphics()
{
	clear_screen();
	restore_colorProfile(0);
	changeColor(GRID_BORDER);
	drawBorders(LENGTH_X);
	for (int cont = 0; cont < LENGTH_Y - 2; cont++)
	{
		printf("%c", WALL);
		changeColor(BACKGROUND);
		for (int spaces = 0; spaces < LENGTH_X - 2; spaces++)
		{
			printf(" ");
		}
		changeColor(GRID_BORDER);
		printf("%c\n", WALL);
	}
	drawBorders(LENGTH_X);
	restore_colorProfile(1);
}

// ---------------------------------------------------------------------
// insertTitle
// ---------------------------------------------------------------------
// Draws the Snake title, the game version and the small snake icon on
// the right of the game field
void insertTitle()
{
	CONSOLE_SCREEN_BUFFER_INFO backupScreenInfo;
	GetConsoleScreenBufferInfo(st, &backupScreenInfo);
	COORD titolo = { INGAME_TITLE_X, INGAME_TITLE_Y };
	SetConsoleCursorPosition(st, titolo);
	restore_colorProfile(0);
	changeColor(GREEN_TEXT);
	printf("Snake "VERSION);
	restore_colorProfile(1);
	COORD tail = { TAIL_TITLE_X, TAIL_TITLE_Y };
	for (int cont = 0; cont < 6; cont++)
	{
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
		if (cont != 5)
		{
			tail.X -= 1;
		}
	}
	for (int cont = 0; cont < 2; cont++)
	{
		tail.Y += 1;
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
	}
	for (int cont = 0; cont < 5; cont++)
	{
		tail.X -= 1;
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
	}
	for (int cont = 0; cont < 2; cont++)
	{
		tail.Y += 1;
		SetConsoleCursorPosition(st, tail);
		printf("%c", SQUARE);
	}
	for (int cont = 0; cont < 7; cont++)
	{
		tail.X += 1;
		SetConsoleCursorPosition(st, tail);
		if (cont != 6)
		{
			printf("%c", SQUARE);
		}
		else
		{
			printf("%c", TRIANGLE_DX);
			printf("  %c", FOOD);
		}
	}
}

// ---------------------------------------------------------------------
// drawSnake
// ---------------------------------------------------------------------
// Draws the snake head pointed in the right direction and calls the
// function that draws its tail
void drawSnake(int flag, int** grid)
{
	SetConsoleCursorPosition(st, Snake.position);
	restore_colorProfile(0);
	changeColor(SNAKE_COLOR);
	switch (Snake.direction)
	{
	case UP: printf("%c", TRIANGLE_UP); break;
	case DOWN: printf("%c", TRIANGLE_DOWN); break;
	case RIGHT: printf("%c", TRIANGLE_DX); break;
	case LEFT: printf("%c", TRIANGLE_SX); break;
	}
	fix_overlappedCharacters(Snake.position, grid);
	if (flag)
	{
		return;
	}
	drawTail(Snake.tail, grid);
	restore_colorProfile(1);
}

// ---------------------------------------------------------------------
// fix_overlappedCharacters
// ---------------------------------------------------------------------
// Checks if the current position is next to a wall or a food item, as in
// that case it would have been slightly overlapped due to a glitch
// in the console graphics. In that case it writes the overlapped character
// again, as the overlapping happens on the right side only and in this
// way the new character on the left is not affected.
void fix_overlappedCharacters(COORD position, int** grid)
{
	if (position.X > LENGTH_X - 3)
	{
		changeColor(DARK_GREEN_TEXT);
		printf("%c", WALL);
		changeColor(SNAKE_COLOR);
		return;
	}
	if (position.X < LENGTH_X - 3)
	{
		if (grid[position.Y - 1][position.X] == FOOD)
		{
			changeColor(CYAN_WHITE);
			printf("%c", FOOD);
			changeColor(SNAKE_COLOR);
		}
	}
}

// ---------------------------------------------------------------------
// fix_overlappedTail
// ---------------------------------------------------------------------
// Recursively explores the coordinates list and corrects the overlapped
// characters, if there are any
void fix_overlappedTail(int** grid, COORD position, TypeTailSCL tail)
{
	if (grid[tail->coordinate.Y - 1][tail->coordinate.X] == 1 &&
		tail->coordinate.X != position.X - 1)
	{
		COORD temp;
		temp.Y = tail->coordinate.Y;
		temp.X = tail->coordinate.X + 1;
		SetConsoleCursorPosition(st, temp);
		printf("%c", SQUARE);
	}
	if (tail->next != NULL)
	{
		fix_overlappedTail(grid, position, tail->next);
	}
}

// ---------------------------------------------------------------------
// updateSnakeGraphics
// ---------------------------------------------------------------------
// Deletes the last tail element from the screen if the snake hasn't eaten
// during the current move, and draws a new tail element next to the tail head
void updateSnakeGraphics(int** grid, int flag)
{
	if (!flag)
	{
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

// ---------------------------------------------------------------------
// drawTail
// ---------------------------------------------------------------------
// Draws all the tail elements when the game starts
void drawTail(TypeTailSCL tailNode, int** grid)
{
	SetConsoleCursorPosition(st, tailNode->coordinate);
	printf("%c", SQUARE);
	fix_overlappedCharacters(tailNode->coordinate, grid);
	if (tailNode->next != NULL)
	{
		drawTail(tailNode->next, grid);
	}
}

// ---------------------------------------------------------------------
// restoreInGameGraphics
// ---------------------------------------------------------------------
// Redraws all the in game menus, the game field, the snake and its tail
// and the food after the user continues playing from the pause menu
void restoreInGameGraphics(int** grid, int points)
{
	initializeGraphics();
	insertTitle();
	drawSnake(0, grid);
	COORD seek_cibo;
	for (int rows = 0; rows < LENGTH_Y - 2; rows++)
	{
		for (int colonne = 0; colonne < LENGTH_X - 2; colonne++)
		{
			if (grid[rows][colonne] == FOOD)
			{
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

// ---------------------------------------------------------------------
// drawgameOver
// ---------------------------------------------------------------------
// Draws the game over text under the game field
void drawGameOver()
{
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

// ---------------------------------------------------------------------
// drawInGameBorder
// ---------------------------------------------------------------------
// Draws the borders of the grid that contains the snake title and the small
// logo, the actual score and the available option to pause the game
void drawInGameBorder()
{
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

// ---------------------------------------------------------------------
// addGridRows
// ---------------------------------------------------------------------
// Draws a row of horizontal bars
void addGridRows(int width)
{
	for (int cont = 0; cont < width; cont++)
	{
		printf("%c", BAR_WtoE);
	}
}

// ---------------------------------------------------------------------
// addGridJunctions
// ---------------------------------------------------------------------
// Adds a row with two junctions on the outside
void addGridJunctions(COORD* border, int width)
{
	border->Y += 1;
	SetConsoleCursorPosition(st, *border);
	printf("%c", BAR_N_S_E);
	addGridRows(width);
	printf("%c", BAR_N_S_W);
}

// ---------------------------------------------------------------------
// addGridColumns
// ---------------------------------------------------------------------
// Draws an ampty row for the current grid, with the delimiting borders
void addGridColumns(int len, COORD* border, int width)
{
	for (int rows = 1; rows < len; rows++)
	{
		border->Y += 1;
		SetConsoleCursorPosition(st, *border);
		printf("%c", BAR_NtoS);
		border->X += width;
		SetConsoleCursorPosition(st, *border);
		printf("%c", BAR_NtoS);
		border->X -= width;
	}
}

// ---------------------------------------------------------------------
// displayPauseInstructions
// ---------------------------------------------------------------------
// Writes the text that informs the user of the possibility to pause the game
void displayPauseInstructions()
{
	COORD pausa = { INGAME_TITLE_X - 1, INGAME_TITLE_Y + 13 };
	SetConsoleCursorPosition(st, pausa);
	printf("Press Esc to");
	pausa.Y += 1;
	SetConsoleCursorPosition(st, pausa);
	printf("pause the game");
}

// ---------------------------------------------------------------------
// updateScore
// ---------------------------------------------------------------------
// Writes the updated score inside the game info grid
void updateScore(int score)
{
	COORD coord_score = { INGAME_TITLE_X, INGAME_TITLE_Y + 10 };
	SetConsoleCursorPosition(st, coord_score);
	restore_colorProfile(0);
	changeColor(DARK_GREEN_TEXT);
	printf("Score: %d", score);
	restore_colorProfile(1);
}

// ---------------------------------------------------------------------
// draw3OptionsGrid
// ---------------------------------------------------------------------
// Draws a grid with three empty rows to display different options
void draw3OptionsGrid(COORD* mainBorder)
{
	changeColor(DARK_TEXT);
	mainBorder->X = X_POSITION_MENU;
	mainBorder->Y = Y_POSITION_MENU;
	SetConsoleCursorPosition(st, *mainBorder);
	openGrid(MENU_WIDTH);
	for (int cont = 0; cont < 3; cont++)
	{
		if (record != 0 && cont == 0)
		{
			addGridColumns(5, mainBorder, MENU_WIDTH + 1);
		}
		else
		{
			addGridColumns(4, mainBorder, MENU_WIDTH + 1);
		}
		if (cont != 2)
		{
			addGridJunctions(mainBorder, MENU_WIDTH);
		}
	}
	closeGrid(MENU_WIDTH, mainBorder);
	changeColor(WHITE_TEXT);
	mainBorder->Y = Y_POSITION_MENU + 2;
	mainBorder->X = X_POSITION_MENU + 3;
}

// ---------------------------------------------------------------------
// mainMenuGraphics
// ---------------------------------------------------------------------
// Displays the game title, the options grid and the available options
void mainMenuGraphics()
{
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
	draw3OptionsGrid(&mainBorder);
	writeOptions("1 ---> New game", "2 ---> Instructions", "0 ---> Exit", 1, &mainBorder);
}

// ---------------------------------------------------------------------
// write options
// ---------------------------------------------------------------------
// Writes the three main options in the correct coordinates, and displays
// the current record if at least a game has been played since the program was launched
void writeOptions(char* option1, char* option2, char* option3, int includeRecord, COORD* border)
{
	for (int cont = 0; cont < 3; cont++)
	{
		SetConsoleCursorPosition(st, *border);
		switch (cont)
		{
		case 0: {
			printf(option1);
			if (includeRecord)
			{
				if (record != 0)
				{
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
		border->Y += 4;
	}
	restore_colorProfile(1);
}

// ---------------------------------------------------------------------
// clearArea
// ---------------------------------------------------------------------
// Clears the console area under the main title, to display other submenus
void clearArea(int x, int y)
{
	DWORD n, size;
	COORD cancella_menu = { x, y };
	size = csbiInfo.dwSize.X * 15;
	FillConsoleOutputCharacter(st, TEXT(' '), size, cancella_menu, &n);
	GetConsoleScreenBufferInfo(st, &csbiInfo);
	FillConsoleOutputAttribute(st, csbiInfo.wAttributes, size, cancella_menu, &n);
}

// ---------------------------------------------------------------------
// graphics_levelSelect
// ---------------------------------------------------------------------
// Displays the grid that contains the available game difficulty levels
int graphics_levelSelect()
{
	clearArea(0, 9);
	COORD position;
	draw3OptionsGrid(&position);
	writeOptions("1 ---> Easy", "2 ---> Normal", "3 ---> Hard", 0, &position);
	int actualSelection;
	while (1)
	{
		actualSelection = _getch();
		if ((actualSelection == 49) || (actualSelection == 50) || (actualSelection == 51))
		{
			switch (actualSelection)
			{
			case 49: return 100;
			case 50: return 80;
			case 51: return 45;
			}
		}
	}
}

// ---------------------------------------------------------------------
// graphics_SelectgameMode
// ---------------------------------------------------------------------
// Draws a two-rows grid and displays the two available game modes: Classic and Snake II
void graphics_SelectGameMode()
{
	clearArea(0, 9);
	changeColor(DARK_TEXT);
	COORD switch_mode;
	switch_mode.X = X_POSITION_MENU;
	switch_mode.Y = Y_POSITION_MENU + 1;
	SetConsoleCursorPosition(st, switch_mode);
	openGrid(MENU_WIDTH);
	for (int cont = 0; cont < 2; cont++)
	{
		addGridColumns(4, &switch_mode, MENU_WIDTH + 1);
		if (cont == 0)
		{
			addGridJunctions(&switch_mode, MENU_WIDTH);
		}
	}
	closeGrid(MENU_WIDTH, &switch_mode);
	changeColor(WHITE_TEXT);
	switch_mode.Y = Y_POSITION_MENU + 3;
	switch_mode.X = X_POSITION_MENU + 3;
	for (int cont = 0; cont < 2; cont++)
	{
		SetConsoleCursorPosition(st, switch_mode);
		switch (cont)
		{
		case 0: printf("1 ---> Classic"); break;
		case 1: printf("2 ---> Snake II"); break;
		}
		switch_mode.Y += 4;
	}
}

// ---------------------------------------------------------------------
// openGrid
// ---------------------------------------------------------------------
// Draws the first line of a grid, with the right junctions on the borders
void openGrid(int length)
{
	printf("%c", BAR_StoE);
	addGridRows(length);
	printf("%c", BAR_StoW);
}

// ---------------------------------------------------------------------
// closeGrid
// ---------------------------------------------------------------------
// Closes a grid by drawing a line and connecting the previous borders with two junctions
void closeGrid(int width, COORD* position)
{
	position->Y += 1;
	SetConsoleCursorPosition(st, *position);
	printf("%c", BAR_NtoE);
	addGridRows(width);
	printf("%c", BAR_WtoN);
}

// ---------------------------------------------------------------------
// createEmptyGrid
// ---------------------------------------------------------------------
// Draws a simple empty grid with the desired width and height
void createEmptyGrid(int length, COORD* start, int height)
{
	SetConsoleCursorPosition(st, *start);
	openGrid(length);
	addGridColumns(height, start, length + 1);
	closeGrid(length, start);
}

// ---------------------------------------------------------------------
// instructions
// ---------------------------------------------------------------------
// Displays the instructions for the game and the dark gray grid around the text
void instructions()
{
	clear_screen();
	COORD border_instructions;
	setMenu(1, &border_instructions, INSTRUCTIONS_WIDTH, 12);
	for (int cont = 0; cont < 6; cont++)
	{
		SetConsoleCursorPosition(st, border_instructions);
		switch (cont)
		{
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

// ---------------------------------------------------------------------
// setMenu
// ---------------------------------------------------------------------
// Writes the correct title for the current section: the pause menu or the instruction page
void setMenu(int flag, COORD* position, int width, int height)
{
	restore_colorProfile(0);
	changeColor(GREEN_TEXT);
	if (flag)
	{
		printf("\n   Snake - Instructions");
	}
	else
	{
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

// ---------------------------------------------------------------------
// pausegraphics
// ---------------------------------------------------------------------
// Writes the text contained inside the grid in the pause menu
void pauseGraphics()
{
	COORD menu_pausa;
	setMenu(0, &menu_pausa, PAUSE_MENU_WIDTH, 7);
	for (int cont = 0; cont < 3; cont++)
	{
		SetConsoleCursorPosition(st, menu_pausa);
		switch (cont)
		{
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