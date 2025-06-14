#include <windows.h>
#include <windowsx.h>
#include <stdio.h>
#include <string.h>
#include "omok_ui.h"
#include "omok_logic.h"
#include "omok_ai.h"
#include "omok_global.h"

WNDPROC oldEditProc;

LRESULT CALLBACK MenuWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE: {
        RECT rect;
        GetClientRect(hwnd, &rect);
        int windowWidth = rect.right - rect.left;
        int buttonWidth = 180;
        int centerX = (windowWidth - buttonWidth) / 2;

        CreateWindow("BUTTON", "플레이어 vs 컴퓨터",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            centerX, 30, buttonWidth, 35,
            hwnd, (HMENU)1, NULL, NULL);

        CreateWindow("BUTTON", "플레이어 vs 플레이어",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            centerX, 80, buttonWidth, 35,
            hwnd, (HMENU)2, NULL, NULL);

        CreateWindow("BUTTON", "종료",
            WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            centerX, 130, buttonWidth, 35,
            hwnd, (HMENU)3, NULL, NULL);
        return 0;
    }

    case WM_COMMAND:
        if (LOWORD(wParam) == 1) {
            HWND hLevelWnd = CreateWindow("LevelWindow", "난이도 선택",
                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 300, 250,
                NULL, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            ShowWindow(hLevelWnd, SW_SHOW);
            ShowWindow(hwnd, SW_HIDE);
        }
        else if (LOWORD(wParam) == 2) {
            memset(board, 0, sizeof(board));
            currentPlayer = 1;
            hoverX = hoverY = -1;
            isGameOver = 0;
            aiLevel = 0;
            gameMode = 2;

            int width = CELL_SIZE * BOARD_SIZE + OFFSET * 2;
            int height = CELL_SIZE * BOARD_SIZE + OFFSET * 2 + 60;
            RECT rect = { 0, 0, width, height };
            AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

            hWndOmok = CreateWindow("OmokWindow", "오목 보드판",
                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                rect.right - rect.left, rect.bottom - rect.top,
                NULL, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            ShowWindow(hWndOmok, SW_SHOW);
            ShowWindow(hwnd, SW_HIDE);
        }
        else if (LOWORD(wParam) == 3) {
            PostQuitMessage(0);
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK LevelWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
    case WM_CREATE:
        CreateWindow("BUTTON", "레벨 2 (상)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 50, 180, 40, hwnd, (HMENU)2, NULL, NULL);
        CreateWindow("BUTTON", "레벨 1 (중)", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
            50, 110, 180, 40, hwnd, (HMENU)1, NULL, NULL);
        return 0;

    case WM_COMMAND:
        if (LOWORD(wParam) == 1 || LOWORD(wParam) == 2) {
            aiLevel = LOWORD(wParam);
            gameMode = 1;
            memset(board, 0, sizeof(board));
            currentPlayer = 1;
            hoverX = hoverY = -1;
            isGameOver = 0;

            int width = CELL_SIZE * BOARD_SIZE + OFFSET * 2;
            int height = CELL_SIZE * BOARD_SIZE + OFFSET * 2 + 60;
            RECT rect = { 0, 0, width, height };
            AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

            hWndOmok = CreateWindow("OmokWindow", "오목 보드판",
                WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
                rect.right - rect.left, rect.bottom - rect.top,
                NULL, NULL, (HINSTANCE)GetWindowLongPtr(hwnd, GWLP_HINSTANCE), NULL);
            ShowWindow(hWndOmok, SW_SHOW);
            ShowWindow(hwnd, SW_HIDE);
        }
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK OmokWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    HDC hdc;
    PAINTSTRUCT ps;

    switch (msg) {
    case WM_LBUTTONDOWN: {
        if (isGameOver) return 0;
        int mouseX = GET_X_LPARAM(lParam);
        int mouseY = GET_Y_LPARAM(lParam);
        int x = (mouseX - OFFSET + CELL_SIZE / 2) / CELL_SIZE;
        int y = (mouseY - OFFSET + CELL_SIZE / 2) / CELL_SIZE;

        placeStone(x, y);
        return 0;
    }

    case WM_MOUSEMOVE: {
        if (!isGameOver) {
            int mouseX = GET_X_LPARAM(lParam);
            int mouseY = GET_Y_LPARAM(lParam);
            int x = (mouseX - OFFSET + CELL_SIZE / 2) / CELL_SIZE;
            int y = (mouseY - OFFSET + CELL_SIZE / 2) / CELL_SIZE;

            if (x >= 0 && x < BOARD_SIZE && y >= 0 && y < BOARD_SIZE) {
                if (hoverX != x || hoverY != y) {
                    hoverX = x;
                    hoverY = y;
                    InvalidateRect(hwnd, NULL, TRUE);  // 전체 화면 새로 그리기
                }
            }
            else {
                if (hoverX != -1 || hoverY != -1) {
                    hoverX = hoverY = -1;
                    InvalidateRect(hwnd, NULL, TRUE);  // 전체 화면 새로 그리기
                }
            }
        }
        return 0;
    }

    case WM_CREATE: {
        hInputBox = CreateWindow("EDIT", "",
            WS_CHILD | WS_VISIBLE | WS_BORDER,
            OFFSET, OFFSET + BOARD_SIZE * CELL_SIZE + 30,
            200, 25, hwnd, (HMENU)INPUTBOX, NULL, NULL);

        hInputLabel = CreateWindow("STATIC", "[x, y] 형식으로 입력해주세요",
            WS_CHILD | WS_VISIBLE,
            OFFSET + 210, OFFSET + BOARD_SIZE * CELL_SIZE + 35,
            200, 20, hwnd, NULL, NULL, NULL);

        oldEditProc = (WNDPROC)SetWindowLongPtr(hInputBox, GWLP_WNDPROC, (LONG_PTR)EditProc);
        return 0;
    }

    case WM_PAINT:
        hdc = BeginPaint(hwnd, &ps);
        SetBkMode(hdc, TRANSPARENT);
        SelectObject(hdc, GetStockObject(DEFAULT_GUI_FONT));

        for (int i = 0; i < BOARD_SIZE; i++) {
            char num[4];
            sprintf_s(num, sizeof(num), "%d", i + 1);
            int x = OFFSET + i * CELL_SIZE;
            int y = OFFSET - 20;

            SIZE textSize;
            GetTextExtentPoint32A(hdc, num, (int)strlen(num), &textSize);
            TextOutA(hdc, x - textSize.cx / 2, y, num, (int)strlen(num));

            x = OFFSET;
            y = OFFSET + i * CELL_SIZE;
            GetTextExtentPoint32A(hdc, num, (int)strlen(num), &textSize);
            TextOutA(hdc, x - CELL_SIZE / 2 - textSize.cx / 2, y - textSize.cy / 2, num, (int)strlen(num));
        }

        for (int i = 0; i < BOARD_SIZE; i++) {
            MoveToEx(hdc, OFFSET + i * CELL_SIZE, OFFSET, NULL);
            LineTo(hdc, OFFSET + i * CELL_SIZE, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE);
            MoveToEx(hdc, OFFSET, OFFSET + i * CELL_SIZE, NULL);
            LineTo(hdc, OFFSET + (BOARD_SIZE - 1) * CELL_SIZE, OFFSET + i * CELL_SIZE);
        }

        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] != 0) {
                    int cx = OFFSET + x * CELL_SIZE;
                    int cy = OFFSET + y * CELL_SIZE;
                    int r = CELL_SIZE / 4;
                    HBRUSH brush = CreateSolidBrush(board[y][x] == 1 ? RGB(0, 0, 0) : RGB(255, 255, 255));
                    HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, brush);
                    Ellipse(hdc, cx - r, cy - r, cx + r, cy + r);
                    SelectObject(hdc, oldBrush);
                    DeleteObject(brush);
                }
            }
        }

        if (hoverX >= 0 && hoverY >= 0 && board[hoverY][hoverX] == 0) {
            int cx = OFFSET + hoverX * CELL_SIZE;
            int cy = OFFSET + hoverY * CELL_SIZE;
            int r = CELL_SIZE / 2 - 2;

            HPEN hPen = CreatePen(PS_SOLID, 1, RGB(180, 180, 180));
            HBRUSH hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
            HPEN oldPen = (HPEN)SelectObject(hdc, hPen);
            HBRUSH oldBrush = (HBRUSH)SelectObject(hdc, hBrush);

            RoundRect(hdc, cx - r, cy - r, cx + r, cy + r, 8, 8);

            SelectObject(hdc, oldPen);
            SelectObject(hdc, oldBrush);
            DeleteObject(hPen);
        }

        EndPaint(hwnd, &ps);
        return 0;

    case WM_USER + 1:
        if (pendingWin > 0) {
            char msg[64];
            if (gameMode == 1) {
                if (pendingWin == 1)
                    strcpy_s(msg, sizeof(msg), "플레이어 승리!");
                else
                    strcpy_s(msg, sizeof(msg), "컴퓨터 승리!");
            }
            else {
                sprintf_s(msg, sizeof(msg), "플레이어 %d 승리!", pendingWin);
            }
            MessageBox(hwnd, msg, "게임 종료", MB_OK | MB_ICONINFORMATION);

            ShowWindow(hInputBox, SW_HIDE);
            ShowWindow(hInputLabel, SW_HIDE);

            int baseX = OFFSET + BOARD_SIZE * CELL_SIZE - 280;
            int baseY = OFFSET + BOARD_SIZE * CELL_SIZE + 30;

            hRestartBtn = CreateWindow("BUTTON", "게임 재시작",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                baseX, baseY, 120, 30, hwnd, (HMENU)200, NULL, NULL);

            hMenuBtn = CreateWindow("BUTTON", "메인 메뉴 이동",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                baseX + 130, baseY, 120, 30, hwnd, (HMENU)201, NULL, NULL);

            pendingWin = 0;
        }
        return 0;

    case WM_COMMAND:
        switch (LOWORD(wParam)) {
        case 200:
            memset(board, 0, sizeof(board));
            currentPlayer = 1;
            isGameOver = 0;
            hoverX = hoverY = -1;
            pendingWin = 0;
            ShowWindow(hRestartBtn, SW_HIDE);
            ShowWindow(hMenuBtn, SW_HIDE);
            ShowWindow(hInputBox, SW_SHOW);
            ShowWindow(hInputLabel, SW_SHOW);
            InvalidateRect(hwnd, NULL, TRUE);
            break;

        case 201:
            ShowWindow(hwnd, SW_HIDE);
            ShowWindow(hwndMenu, SW_SHOW);
            break;
        }
        return 0;

    case WM_CLOSE:
        DestroyWindow(hwnd);
        return 0;

    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

LRESULT CALLBACK EditProc(HWND hwndEdit, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (msg == WM_KEYDOWN && wParam == VK_RETURN) {
        char buffer[32];
        GetWindowText(hwndEdit, buffer, sizeof(buffer));
        int x = 0, y = 0;
        if (sscanf_s(buffer, "%d,%d", &x, &y) == 2 || sscanf_s(buffer, "[%d,%d]", &x, &y) == 2) {
            x -= 1; y -= 1;
            placeStone(x, y);
            SetWindowText(hwndEdit, "");
        }
        else {
            MessageBox(GetParent(hwndEdit), "입력 형식이 잘못되었습니다. 예: 5,7 또는 [5,7]", "입력 오류", MB_OK | MB_ICONWARNING);
        }
        return 0;
    }
    return CallWindowProc(oldEditProc, hwndEdit, msg, wParam, lParam);
}