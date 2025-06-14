#include <windows.h>
#include <tchar.h>
#include "omok_logic.h"
#include "omok_global.h"

int countStones(int x, int y, int dx, int dy) {
    int count = 1, player = board[y][x];
    int nx = x + dx, ny = y + dy;
    while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[ny][nx] == player) {
        count++; nx += dx; ny += dy;
    }
    nx = x - dx; ny = y - dy;
    while (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[ny][nx] == player) {
        count++; nx -= dx; ny -= dy;
    }
    return count;
}

int detectOpenSequence(int x, int y, int player, int length) {
    int dirs[4][2] = { {1,0},{0,1},{1,1},{-1,1} };
    int count = 0;

    for (int d = 0; d < 4; d++) {
        int cnt = 1;
        int blocks = 0;

        // 정방향
        for (int i = 1; i < length; i++) {
            int nx = x + dirs[d][0] * i, ny = y + dirs[d][1] * i;
            if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) {
                blocks++;
                break;
            }
            if (board[ny][nx] == player) cnt++;
            else if (board[ny][nx] != 0) { blocks++; break; }
            else break;
        }

        // 역방향
        for (int i = 1; i < length; i++) {
            int nx = x - dirs[d][0] * i, ny = y - dirs[d][1] * i;
            if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) {
                blocks++;
                break;
            }
            if (board[ny][nx] == player) cnt++;
            else if (board[ny][nx] != 0) { blocks++; break; }
            else break;
        }

        if (cnt == length && blocks == 0) count++; // 열린 패턴만
    }
    return count;
}

int checkWin(int x, int y) {
    int dirs[4][2] = { {1,0},{0,1},{1,1},{-1,1} };
    for (size_t i = 0; i < 4; i++) {
        int count = countStones(x, y, dirs[i][0], dirs[i][1]);
        if (count == 5) return 1; // 승리
        if (currentPlayer == 1 && count > 5) return -1; // 장목 금지
    }

    // 금수 감지 (흑만)
    if (currentPlayer == 1) {
        int openFour = detectOpenSequence(x, y, 1, 4);
        int openThree = detectOpenSequence(x, y, 1, 3);
        if (openFour >= 2) return -3;  // 44 금수
        if (openThree >= 2) return -2; // 33 금수
    }

    return 0;
}

void placeStone(int x, int y) {
    if (board[y][x] != 0 || isGameOver) return;

    board[y][x] = currentPlayer;
    
    // 금수 사전 감지
    int winCheck = checkWin(x, y);

    if (winCheck == 1) {
        isGameOver = 1;
        pendingWin = currentPlayer;
        InvalidateRect(hWndOmok, NULL, TRUE);
        PostMessage(hWndOmok, WM_USER + 1, 0, 0);
    }
    else if (winCheck < 0) {
        board[y][x] = 0; // 돌 되돌리기

        if (winCheck == -1)
            MessageBox(hWndOmok, _T("장목 금지! 흑은 6개 이상 놓을 수 없습니다."), _T("반칙"), MB_OK | MB_ICONWARNING);
        else if (winCheck == -2)
            MessageBox(hWndOmok, _T("33 금수입니다! 흑은 동시에 열린 3을 둘 수 없습니다."), _T("반칙"), MB_OK | MB_ICONWARNING);
        else if (winCheck == -3)
            MessageBox(hWndOmok, _T("44 금수입니다! 흑은 동시에 열린 4를 둘 수 없습니다."), _T("반칙"), MB_OK | MB_ICONWARNING);
    }
    else {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        InvalidateRect(hWndOmok, NULL, TRUE);

        if (gameMode == 1 && currentPlayer == 2 && !isGameOver) {
            aiMove(); // AI 수 실행
        }
    }
}
