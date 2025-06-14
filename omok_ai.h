#pragma once
#ifndef OMOK_AI_H
#define OMOK_AI_H

#include <windows.h>

#define BOARD_SIZE 19

extern int board[BOARD_SIZE][BOARD_SIZE];
extern int currentPlayer;
extern int isGameOver;
extern HWND hWndOmok;
extern int aiLevel;

void aiMove();
int evaluate(int x, int y, int player);

#endif