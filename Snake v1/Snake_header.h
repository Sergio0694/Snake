/* ============================================================================
*  Snake_header.h
* ============================================================================

*  Author:         (c) 2014 Sergio Pedri
*  License:        See the end of this file for license information
*  Created:        July 6, 2014

*  Revision:       2.0
*/

#ifndef SNAKE_H
#define SNAKE_H

//Typedef of variables to create the Snake and modify it

struct SnakeSCL {
	COORD coordinate;
	struct SnakeSCL* previous;
	struct SnakeSCL* next;
};

typedef struct SnakeSCL TailSCL;
typedef TailSCL* TypeTailSCL;

typedef enum { UP, DOWN, RIGHT, LEFT } xy;

typedef struct {
	COORD position;
	TypeTailSCL tail;
	TypeTailSCL tailEnd;
	COORD backup_tailEnd;
	xy direction;
} SnakeInfo;

//Global variables that are declared in the main file

extern HANDLE st;
extern CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
extern WORD colorBackup;
extern SnakeInfo Snake;
extern int record;

//Directional arrows

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_RIGHT 77
#define KEY_LEFT 75

//Macros for the main grid and stuff

#define LENGTH_X 40
#define LENGTH_Y 19
#define INGAME_TITLE_X 50
#define INGAME_TITLE_Y 3
#define TAIL_TITLE_X 62
#define TAIL_TITLE_Y 5
#define START_POSITION_X 15
#define START_POSITION_Y 14
#define BORDER_LENGTH_X 19
#define MENU_WIDTH 25
#define EXIT -1
#define INVALID_MOVE 99
#define VERSION "   v2.0"
#define X_POSITION_MENU 23
#define Y_POSITION_MENU 10
#define INSTRUCTIONS_WIDTH 65
#define PAUSE_MENU_WIDTH 50

//ASCII symbols

#define WALL 177
#define SQUARE 254
#define TRIANGLE_DX 16
#define TRIANGLE_SX 17
#define TRIANGLE_UP 30
#define TRIANGLE_DOWN 31
#define FOOD 15
#define BAR_NtoS 186
#define BAR_WtoE 205
#define BAR_StoE 201
#define BAR_StoW 187
#define BAR_WtoN 188
#define BAR_NtoE 200
#define BAR_N_S_E 204
#define BAR_N_S_W 185

//Text colors in the DOS console

#define LIGHT_GREY 383
#define BLACK 368
#define DARK_GREEN 370
#define DARK_TEXT 264
#define WHITE_TEXT 271
#define GREEN_TEXT 266
#define RED_TEXT 268
#define DARK_GREEN_TEXT 258
#define CYAN_BLACK 304
#define CYAN_WHITE 319
#define GRID_BORDER DARK_GREEN_TEXT
#define SNAKE_COLOR CYAN_BLACK
#define BACKGROUND SNAKE_COLOR
#define FOOD_COLOR CYAN_WHITE

//Prototypes of graphics functions

void clear_screen();
void changeColor(int color);
void restore_colorProfile(int flag);
void drawBorders(int size);
void initializeGraphics();
void insertTitle();
void drawSnake(int flag, int** grid);
void drawTail(TypeTailSCL tailNode, int** grid);
void updateSnakeGraphics(int** grid, int flag);
void updateTailGraphics(TypeTailSCL tailNode, int** grid);
void restoreInGameGraphics(int** grid, int points);
void drawGameOver();
void drawInGameBorder();
void addGridRows(int width);
void addGridColumns(int len, COORD* border, int width);
void addGridJunctions(COORD* border, int width);
void displayPauseInstructions();
void updateScore(int score);
void mainMenuGraphics();
void closeGrid(int width, COORD* position);
void openGrid(int length);
void createEmptyGrid(int length, COORD* start, int height);
void instructions();
void pauseGraphics();
void setMenu(int flag, COORD* position, int width, int height);
void fix_overlappedCharacters(COORD position, int** grid);
void fix_overlappedTail(int** grid, COORD position, TypeTailSCL tail);
void graphics_SelectGameMode();
void draw3OptionsGrid(COORD* mainBorder, int levelSelect);
void clearArea(int x, int y);
int graphics_levelSelect();
void writeOptions(char* option1, char* option2, char* option3, int includeRecord, COORD* border);

//Prototypes of game logic funtions

void newGame(int** grid, int* score, int* backToMenu, int gameMode, int delay);
int mainMenu(int flag);
int** initializeMatrix();
void restoreMatrix(int** matrix);
void addFood(int** grid);
void initializeNewGame(int** grid, int* score, int* force_exit);
void allocateTail(TypeTailSCL* tailNode, TypeTailSCL previous, int length);
void initializeTail(TypeTailSCL tailNode, int coord_X);
void assignSnake2Matrix(int** grid, int flag);
void assignTail2Matrix(int** grid, TypeTailSCL tailNode, int flag);
int playerMove();
void muoviSnake(int direction, int mode);
void scaleTailCoordinates(TypeTailSCL tailNode);
int checkValidMove(int move);
int gameOverCheck(int direction, int mode);
int collision_check(COORD nextPosition, TailSCL* tailNode);
void fattenSnake();
int checkIfEaten(int** grid);
void restoreSnakeLength();
void deallocateSCL(TailSCL* tailNode);
void updateDirection(int pressedKey, xy* assign);
void translateCoordinates(xy direction, COORD* position);

#endif

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