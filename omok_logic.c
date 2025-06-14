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

        // ������
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

        // ������
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

        if (cnt == length && blocks == 0) count++; // ���� ���ϸ�
    }
    return count;
}

int checkWin(int x, int y) {
    int dirs[4][2] = { {1,0},{0,1},{1,1},{-1,1} };
    for (size_t i = 0; i < 4; i++) {
        int count = countStones(x, y, dirs[i][0], dirs[i][1]);
        if (count == 5) return 1; // �¸�
        if (currentPlayer == 1 && count > 5) return -1; // ��� ����
    }

    // �ݼ� ���� (�游)
    if (currentPlayer == 1) {
        int openFour = detectOpenSequence(x, y, 1, 4);
        int openThree = detectOpenSequence(x, y, 1, 3);
        if (openFour >= 2) return -3;  // 44 �ݼ�
        if (openThree >= 2) return -2; // 33 �ݼ�
    }

    return 0;
}

void placeStone(int x, int y) {
    if (board[y][x] != 0 || isGameOver) return;

    board[y][x] = currentPlayer;
    
    // �ݼ� ���� ����
    int winCheck = checkWin(x, y);

    if (winCheck == 1) {
        isGameOver = 1;
        pendingWin = currentPlayer;
        InvalidateRect(hWndOmok, NULL, TRUE);
        PostMessage(hWndOmok, WM_USER + 1, 0, 0);
    }
    else if (winCheck < 0) {
        board[y][x] = 0; // �� �ǵ�����

        if (winCheck == -1)
            MessageBox(hWndOmok, _T("��� ����! ���� 6�� �̻� ���� �� �����ϴ�."), _T("��Ģ"), MB_OK | MB_ICONWARNING);
        else if (winCheck == -2)
            MessageBox(hWndOmok, _T("33 �ݼ��Դϴ�! ���� ���ÿ� ���� 3�� �� �� �����ϴ�."), _T("��Ģ"), MB_OK | MB_ICONWARNING);
        else if (winCheck == -3)
            MessageBox(hWndOmok, _T("44 �ݼ��Դϴ�! ���� ���ÿ� ���� 4�� �� �� �����ϴ�."), _T("��Ģ"), MB_OK | MB_ICONWARNING);
    }
    else {
        currentPlayer = (currentPlayer == 1) ? 2 : 1;
        InvalidateRect(hWndOmok, NULL, TRUE);

        if (gameMode == 1 && currentPlayer == 2 && !isGameOver) {
            aiMove(); // AI �� ����
        }
    }
}
