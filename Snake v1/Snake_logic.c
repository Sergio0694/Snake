/* ============================================================================
*  Snake_logic.c
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

int mainMenu(int flag) {
	int actualSelection;
	while (1) {
		actualSelection = _getch();
		if ((actualSelection == 48) || (actualSelection == 49) || (actualSelection == 50)) {
			if (flag == 1) {
				switch (actualSelection) {
				case 48: return 0;
				case 49: return 1;
				case 50: return 2;
				}
			}
			else if (!flag) {
				switch (actualSelection) {
				case 48: return 0;
				case 49: return 1;
				}
			}
			else {
				switch (actualSelection) {
				case 49: return 1;
				case 50: return 2;
				}
			}
		}
		else if (actualSelection == 27) {
			return 0;
		}
	}
	return 0;
}

void initializeNewGame(int** grid, int* score, int* force_exit) {
	srand((unsigned)time(NULL));
	Snake.position.X = START_POSITION_X;
	Snake.position.Y = START_POSITION_Y;
	Snake.direction = RIGHT;
	Snake.backup_tailEnd.X = START_POSITION_X - 2;
	Snake.backup_tailEnd.Y = START_POSITION_Y;
	initializeTail(Snake.tail, START_POSITION_X - 1);
	restoreMatrix(grid);
	assignSnake2Matrix(grid, 1);
	initializeGraphics();
	*score = 0;
	*force_exit = 0;
}

void initializeTail(TypeTailSCL tailNode, int coord_X) {
	tailNode->coordinate.X = coord_X;
	tailNode->coordinate.Y = START_POSITION_Y;
	if (tailNode->next != NULL) {
		initializeTail(tailNode->next, coord_X - 1);
	}
	else {
		Snake.tailEnd = tailNode;
	}
}

void allocateTail(TypeTailSCL* tailNode, TypeTailSCL previous, int length) {
	*tailNode = (TailSCL*)malloc(sizeof(TailSCL));
	(*tailNode)->previous = previous;
	if (length != 1) {
		allocateTail(&(*tailNode)->next, *tailNode, length + 1);
	}
	else {
		(*tailNode)->next = NULL;
	}
}

void assignSnake2Matrix(int** grid, int flag) {
	grid[Snake.position.Y - 1][Snake.position.X - 1] = flag;
	assignTail2Matrix(grid, Snake.tail, flag);
}

void assignTail2Matrix(int** grid, TypeTailSCL tailNode, int flag) {
	grid[tailNode->coordinate.Y - 1][tailNode->coordinate.X - 1] = flag;
	if (tailNode->next != NULL) {
		assignTail2Matrix(grid, tailNode->next, flag);
	}
}

int** initializeMatrix() {
	int** matrix = (int**)malloc(sizeof(int*)*(LENGTH_Y - 2));
	for (int rows = 0; rows < LENGTH_Y - 2; rows++) {
		matrix[rows] = (int*)malloc(sizeof(int)*(LENGTH_X - 2));
	}
	return matrix;
}

void restoreMatrix(int** matrix) {
	for (int rows = 0; rows < LENGTH_Y - 2; rows++) {
		for (int column = 0; column < LENGTH_X - 2; column++) {
			matrix[rows][column] = 0;
		}
	}
}

void addFood(int** grid) {
	int coord_X, coord_Y;
	while (1) {
		coord_X = rand() % (LENGTH_X - 2);
		coord_Y = rand() % (LENGTH_Y - 2);
		if (grid[coord_Y][coord_X] == 0) {
			break;
		}
	}
	grid[coord_Y][coord_X] = FOOD;
	COORD foodDrop;
	foodDrop.Y = coord_Y + 1;
	foodDrop.X = coord_X + 1;
	SetConsoleCursorPosition(st, foodDrop);
	restore_colorProfile(0);
	changeColor(FOOD_COLOR);
	printf("%c", FOOD);
	restore_colorProfile(1);
}

int playerMove() {
	int ch, temp_move;
	ch = _getch();
	if ((ch == 0) || (ch == 224)) {
		temp_move = _getch();
		if ((temp_move != 72) && (temp_move != 75) &&
			(temp_move != 77) && (temp_move != 80)) {
			return INVALID_MOVE;
		}
		return temp_move;
	}
	else if (ch == 27) {
		return EXIT;
	}
	else {
		return INVALID_MOVE;
	}
}

int checkValidMove(int move) {
	if ((move == KEY_UP && Snake.direction == DOWN) ||
		(move == KEY_DOWN && Snake.direction == UP) ||
		(move == KEY_RIGHT && Snake.direction == LEFT) ||
		(move == KEY_LEFT && Snake.direction == RIGHT)) {
		return 0;
	}
	return 1;
}

int gameOverCheck(int direction, int mode) {
	COORD border_test = Snake.position;
	xy direction_temp;
	updateDirection(direction, &direction_temp);
	if (!mode) {
		switch (direction) {
		case KEY_UP: {
			border_test.Y = Snake.position.Y - 1;
			break;
		}
		case KEY_DOWN: {
			border_test.Y = Snake.position.Y + 1;
			break;
		}
		case KEY_RIGHT: {
			border_test.X = Snake.position.X + 1;
			break;
		}
		case KEY_LEFT: {
			border_test.X = Snake.position.X - 1;
			break;
		}
		}
		if ((border_test.X - 1) < 0 || (border_test.X - 1) > (LENGTH_X - 3) ||
			(border_test.Y - 1) < 0 || (border_test.Y - 1) > (LENGTH_Y - 3)) {
			return 1;
		}
	}
	else {
		translateCoordinates(direction_temp, &border_test);
	}
	return collision_check(border_test, Snake.tail);
}

int collision_check(COORD nextPosition, TailSCL* tailNode) {
	if ((nextPosition.X == tailNode->coordinate.X) &&
		(nextPosition.Y == tailNode->coordinate.Y) &&
		tailNode->next != NULL) {
		return 1;
	}
	if (tailNode->next == NULL) {
		return 0;
	}
	else {
		return collision_check(nextPosition, tailNode->next);
	}
}

void updateDirection(int pressedKey, xy* assign) {
	switch (pressedKey) {
	case KEY_UP: *assign = UP; break;
	case KEY_DOWN: *assign = DOWN; break;
	case KEY_RIGHT: *assign = RIGHT; break;
	case KEY_LEFT: *assign = LEFT; break;
	}
}

void muoviSnake(int direction, int mode) {
	scaleTailCoordinates(Snake.tailEnd);
	updateDirection(direction, &(Snake.direction));
	if (!mode) {
		switch (Snake.direction) {
		case UP: Snake.position.Y -= 1; break;
		case DOWN: Snake.position.Y += 1; break;
		case RIGHT: Snake.position.X += 1; break;
		case LEFT: Snake.position.X -= 1; break;
		}
	}
	else {
		translateCoordinates(Snake.direction, &(Snake.position));
	}
}

void translateCoordinates(xy direction, COORD* position) {
	switch (direction) {
	case UP: {
		if (position->Y > 1) {
			position->Y -= 1;
		}
		else {
			position->Y = LENGTH_Y - 2;
		}
		break;
	}
	case DOWN: {
		if (position->Y < LENGTH_Y - 2) {
			position->Y += 1;
		}
		else {
			position->Y = 1;
		}
		break;
	}
	case RIGHT: {
		if (position->X < LENGTH_X - 2) {
			position->X += 1;
		}
		else {
			position->X = 1;
		}
		break;
	}
	case LEFT: {
		if (position->X > 1) {
			position->X -= 1;
		}
		else {
			position->X = LENGTH_X - 2;
		}
		break;
	}
	}
}

int checkIfEaten(int** grid) {
	if (grid[Snake.position.Y - 1][Snake.position.X - 1] == FOOD) {
		return 1;
	}
	return 0;
}

void scaleTailCoordinates(TypeTailSCL tailNode) {
	if (tailNode->previous != NULL) {
		if (tailNode->next == NULL) {
			Snake.backup_tailEnd = tailNode->coordinate;
		}
		tailNode->coordinate = tailNode->previous->coordinate;
		scaleTailCoordinates(tailNode->previous);
	}
	else {
		tailNode->coordinate = Snake.position;
	}
}

void fattenSnake() {
	Snake.tailEnd->next = (TailSCL*)malloc(sizeof(TailSCL));
	Snake.tailEnd->next->previous = Snake.tailEnd;
	Snake.tailEnd = Snake.tailEnd->next;
	Snake.tailEnd->next = NULL;
	Snake.tailEnd->coordinate = Snake.backup_tailEnd;
}

void restoreSnakeLength() {
	if (Snake.tail->next->next == NULL) {
		return;
	}
	TailSCL* temp = Snake.tail->next;
	Snake.tailEnd = temp;
	temp = temp->next;
	Snake.tailEnd->next = NULL;
	Snake.backup_tailEnd = temp->coordinate;
	deallocateSCL(temp);
}

void deallocateSCL(TailSCL* tailNode) {
	TailSCL* temp_next = tailNode->next;
	free(tailNode);
	if (temp_next != NULL) {
		deallocateSCL(temp_next);
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