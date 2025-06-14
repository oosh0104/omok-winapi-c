#pragma once
#ifndef OMOK_LOGIC_H
#define OMOK_LOGIC_H

int checkWin(int x, int y);
int countStones(int x, int y, int dx, int dy);
int detectOpenSequence(int x, int y, int player, int length);
void placeStone(int x, int y);

#endif