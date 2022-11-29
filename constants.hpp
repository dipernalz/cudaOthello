#ifndef CONSTANTS_H
#define CONSTANTS_H

// Board is 8x8
#define N 8

// States of squares on the board
#define EMPTY false
#define TAKEN true

// Colors of pieces
#define BLACK false
#define WHITE true

#define BOARD_INDEX(r, c) (r * N + c)
#define POS_IN_BOARD(r, c) (r >= 0 && r < N && c >= 0 && c < N)

#endif
