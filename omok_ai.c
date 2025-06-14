#include "omok_ai.h"
#include "omok_logic.h"
#include "omok_global.h"
#include <stdlib.h>
#include <time.h>

void aiMove() {
    int x, y;

    if (aiLevel == 1) {  // 1단계 (중간 AI)
        if ((rand() % 100) < 10) {  // 10% 확률로 내 돌 주변 1칸 랜덤
            for (int tries = 0; tries < 100; tries++) {
                for (int i = 0; i < BOARD_SIZE; i++) {
                    for (int j = 0; j < BOARD_SIZE; j++) {
                        if (board[i][j] == 2) {
                            int dx = (rand() % 3) - 1;
                            int dy = (rand() % 3) - 1;
                            int nx = j + dx;
                            int ny = i + dy;
                            if (nx >= 0 && nx < BOARD_SIZE && ny >= 0 && ny < BOARD_SIZE && board[ny][nx] == 0) {
                                placeStone(nx, ny);
                                return;
                            }
                        }
                    }
                }
            }
            do {
                x = rand() % BOARD_SIZE;
                y = rand() % BOARD_SIZE;
            } while (board[y][x] != 0);
            placeStone(x, y);
            return;
        }

        int maxScore = 0, bestX = -1, bestY = -1;
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] != 0) continue;
                int attack = evaluate(x, y, 2);
                int defense = evaluate(x, y, 1);
                int score = (int)(attack * 0.6 + defense * 0.3);

                if (score > maxScore) {
                    maxScore = score;
                    bestX = x;
                    bestY = y;
                }
            }
        }
        if (bestX != -1 && bestY != -1) placeStone(bestX, bestY);
    }

    else if (aiLevel == 2) {  // 2단계 (상급 AI)
        int maxScore = 0, bestX = -1, bestY = -1;
        for (int y = 0; y < BOARD_SIZE; y++) {
            for (int x = 0; x < BOARD_SIZE; x++) {
                if (board[y][x] != 0) continue;
                int attack = evaluate(x, y, 2);
                int defense = evaluate(x, y, 1);
                int score = (attack > defense) ? attack : (int)(defense * 0.7);

                if (score > maxScore) {
                    maxScore = score;
                    bestX = x;
                    bestY = y;
                }
            }
        }
        if (bestX != -1 && bestY != -1) placeStone(bestX, bestY);
    }
}

int evaluate(int x, int y, int player) {
    int score = 0;
    int dirs[4][2] = { {1,0}, {0,1}, {1,1}, {-1,1} };
    for (int d = 0; d < 4; d++) {
        int count = 1, block = 0;
        for (int i = 1; i < 5; i++) {
            int nx = x + dirs[d][0] * i, ny = y + dirs[d][1] * i;
            if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) break;
            if (board[ny][nx] == player) count++;
            else if (board[ny][nx] != 0) { block++; break; }
            else break;
        }
        for (int i = 1; i < 5; i++) {
            int nx = x - dirs[d][0] * i, ny = y - dirs[d][1] * i;
            if (nx < 0 || ny < 0 || nx >= BOARD_SIZE || ny >= BOARD_SIZE) break;
            if (board[ny][nx] == player) count++;
            else if (board[ny][nx] != 0) { block++; break; }
            else break;
        }

        if (count >= 5) return 1000000;
        if (block == 0) {
            if (count == 4) score += 100000;
            else if (count == 3) score += 1000;
            else if (count == 2) score += 100;
            else if (count == 1) score += 10;
        }
        else if (block == 1) {
            if (count == 4) score += 10000;
            else if (count == 3) score += 500;
            else if (count == 2) score += 50;
            else if (count == 1) score += 5;
        }
    }
    return score;
}