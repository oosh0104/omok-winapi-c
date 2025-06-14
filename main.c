#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <windowsx.h>
#include <tchar.h>
#include <stdio.h>
#include <string.h>
#include "omok_ui.h"
#include "omok_ai.h"
#include "omok_global.h"

int board[BOARD_SIZE][BOARD_SIZE] = { 0 };
int currentPlayer = 1;
int isGameOver = 0;
int gameMode = 0;
int hoverX = -1, hoverY = -1;
int pendingWin = 0;
int aiLevel = 1;

HWND hWndOmok = NULL;
HWND hInputBox = NULL;
HWND hInputLabel = NULL;
HWND hwndMenu = NULL;
HWND hRestartBtn = NULL;
HWND hMenuBtn = NULL;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
    LPSTR lpCmdLine, int nCmdShow)
{
    WNDCLASS wc_menu = { 0 };
    wc_menu.lpfnWndProc = MenuWndProc;
    wc_menu.hInstance = hInstance;
    wc_menu.lpszClassName = "MenuWindow";
    wc_menu.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc_menu.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc_menu);

    WNDCLASS wc_level = { 0 };
    wc_level.lpfnWndProc = LevelWndProc;
    wc_level.hInstance = hInstance;
    wc_level.lpszClassName = "LevelWindow";
    wc_level.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc_level.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc_level);

    WNDCLASS wc_omok = { 0 };
    wc_omok.lpfnWndProc = OmokWndProc;
    wc_omok.hInstance = hInstance;
    wc_omok.lpszClassName = "OmokWindow";
    wc_omok.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc_omok.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    RegisterClass(&wc_omok);

    hwndMenu = CreateWindow("MenuWindow", "오목 메뉴", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 300, 240, NULL, NULL, hInstance, NULL);

    ShowWindow(hwndMenu, nCmdShow);
    UpdateWindow(hwndMenu);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return 0;
}
