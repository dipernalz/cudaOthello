#include "othello.hpp"

#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <iostream>

// Board is 8x8
#define N 8

// States of squares on the board
#define EMPTY false
#define TAKEN true

// Colors of pieces
#define BLACK false
#define WHITE true

/* static helper functions */
static inline uint8_t board_index(uint8_t row, uint8_t col) {
    return N * row + col;
}

/* public class methods */
othello::othello() {
    black = (bool *)calloc(64, sizeof(bool));
    black[board_index(4, 3)] = TAKEN;
    black[board_index(3, 4)] = TAKEN;

    white = (bool *)calloc(64, sizeof(bool));
    white[board_index(3, 3)] = TAKEN;
    white[board_index(4, 4)] = TAKEN;

    turn = BLACK;
}

void othello::board_free() {
    free(black);
    free(white);
}

othello *othello::next_boards(uint8_t *n_found) {
    bool *board_a, *board_b;
    if (turn == BLACK) {
        board_a = black;
        board_b = white;
    } else {
        board_a = white;
        board_b = black;
    }

    uint8_t n = 0;
    othello *next_boards = (othello *)malloc(N * N * sizeof(othello));
    for (int8_t r = 0; r < N; r++) {
        for (int8_t c = 0; c < N; c++) {
            if (!board_a[board_index(r, c)]) continue;
            for (int8_t dr = -1; dr <= 1; dr++) {
                for (int8_t dc = -1; dc <= 1; dc++) {
                    if (dr == 0 && dc == 0) continue;
                    int8_t tr = r + dr;
                    int8_t tc = c + dc;
                    while (tr >= 0 && tr < N && tc >= 0 && tc < N &&
                           board_b[board_index(tr, tc)]) {
                        tr += dr;
                        tc += dc;
                    }
                    if (tr >= 0 && tr < N && tc >= 0 && tc < N &&
                        (tr != r + dr || tc != c + dc) &&
                        !board_a[board_index(tr, tc)]) {
                        next_boards[n] = *this;
                        bool *next_board_a, *next_board_b;
                        if (next_boards[n].turn == BLACK) {
                            next_board_a = next_boards[n].black;
                            next_board_b = next_boards[n].white;
                        } else {
                            next_board_a = next_boards[n].white;
                            next_board_b = next_boards[n].black;
                        }
                        do {
                            next_board_a[board_index(tr, tc)] = TAKEN;
                            next_board_b[board_index(tr, tc)] = EMPTY;
                            tr -= dr;
                            tc -= dc;
                        } while (tr != r || tc != c);
                        next_boards[n].turn = !next_boards[n].turn;
                        n++;
                    }
                }
            }
        }
    }

    *n_found = n;
    return next_boards;
}

void othello::print() {
    for (uint8_t r = 0; r < N; r++) {
        for (uint8_t c = 0; c < N; c++) {
            assert(!black[board_index(r, c)] || !white[board_index(r, c)]);
            char piece = black[board_index(r, c)]
                             ? 'B'
                             : (white[board_index(r, c)] ? 'W' : '.');
            std::cout << piece << ' ';
        }
        std::cout << std::endl;
    }
    std::cout << (turn == BLACK ? "BLACK" : "WHITE") << "'s turn" << std::endl;
}
