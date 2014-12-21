/* ============================================================================
*  Snake.c
* ============================================================================

*  Author:         (c) 2014 Sergio Pedri
*  License:        See the end of this file for license information
*  Created:        July 6, 2014

*  Revision:       2.0
*/

#include <stdio.h>
#include <stdlib.h>
#include <Windows.h>
#include <conio.h>
#include <time.h>
#include "Snake_header.h"

HANDLE st;
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;
WORD colorBackup;
SnakeInfo Snake;
int record = 0;

void main() {
	int** grid = initializeMatrix();
	int score, force_exit, actualSelection = -1;
	st = GetStdHandle(STD_OUTPUT_HANDLE);
	restore_colorProfile(0);
	allocateTail(&(Snake.tail), NULL,  0);
	while (actualSelection != 0) {
		mainMenuGraphics();
		actualSelection = mainMenu(1);
		if (actualSelection == 1) {
			actualSelection = 0;
			graphics_SelectGameMode();
			while ((actualSelection != 1) && (actualSelection != 2)) {
				actualSelection = mainMenu(2);
			}
			int gameMode = graphics_levelSelect();
			initializeNewGame(grid, &score, &force_exit);
			insertTitle();
			drawInGameBorder();
			displayPauseInstructions();
			drawSnake(0, grid);
			updateScore(score);
			assignSnake2Matrix(grid, 1);
			addFood(grid);
			newGame(grid, &score, &force_exit, actualSelection - 1, gameMode);
			if (!force_exit) {
				_getch();
			}
			clear_screen();
			restoreSnakeLength();
		}
		else if (actualSelection == 2) {
			instructions();
		}
	}
}

void newGame(int** grid, int* score, int* backToMenu, int gameMode, int delay) {
	int eaten_flag, skip_sleep = 0;
	int backup_move = KEY_RIGHT, move = KEY_RIGHT;
	while (1) {
		eaten_flag = 0;
		if (!skip_sleep) {
			if (move == KEY_RIGHT || move == KEY_LEFT) {
				Sleep(delay - 20);
			}
			else {
				Sleep(delay);
			}
		}
		else {
			skip_sleep = 0;
		}
		if (_kbhit()) {
			move = playerMove();
		}
		if (move == EXIT) {
			while (1) {
				clear_screen();
				pauseGraphics();
				if (mainMenu(0)) {
					break;
				}
				else {
					*backToMenu = 1;
					return;
				}
			}
			move = backup_move;
			restoreInGameGraphics(grid, *score);
			continue;
		}
		if (!checkValidMove(move) || move == INVALID_MOVE) {
			move = backup_move;
			skip_sleep = 1;
			continue;
		}
		backup_move = move;
		if (gameOverCheck(move, gameMode)) {
			drawGameOver();
			Sleep(1000);
			if (*score > record) {
				record = *score;
			}
			break;
		}
		assignSnake2Matrix(grid, 0);
		muoviSnake(move, gameMode);
		eaten_flag = checkIfEaten(grid);
		if (eaten_flag) {
			fattenSnake();
		}
		assignSnake2Matrix(grid, 1);
		if (eaten_flag) {
			addFood(grid);
			updateSnakeGraphics(grid, 1);
			*score += 1;
			updateScore(*score);
		}
		else {
			updateSnakeGraphics(grid, 0);
		}
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