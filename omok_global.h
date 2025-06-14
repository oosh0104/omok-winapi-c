#pragma once

#include <windows.h>

#define BOARD_SIZE 19
#define CELL_SIZE 25
#define OFFSET 40
#define INPUTBOX 100

// ���� ���� ���� ������
extern int board[BOARD_SIZE][BOARD_SIZE];
extern int currentPlayer;
extern int isGameOver;
extern int gameMode;
extern int hoverX, hoverY;
extern int pendingWin;
extern int aiLevel;

// ���� HWND �ڵ�
extern HWND hWndOmok;
extern HWND hInputBox;
extern HWND hInputLabel;
extern HWND hwndMenu;
extern HWND hRestartBtn;
extern HWND hMenuBtn;
