#include "othello.hpp"

#include <cassert>
#include <cinttypes>
#include <cstdlib>
#include <cstring>
#include <iostream>

#include "constants.hpp"

#define BOARD_INDEX(r, c) (r * N + c)
#define POS_IN_BOARD(r, c) (r >= 0 && r < N && c >= 0 && c < N)

/* othello - private class methods */
bool othello::make_move(uint8_t row, uint8_t col) {
    bool *board_a, *board_b;
    uint8_t *n_color_a, *n_color_b;
    if (turn == BLACK) {
        board_a = black;
        board_b = white;
        n_color_a = &n_black;
        n_color_b = &n_white;
    } else {
        board_a = white;
        board_b = black;
        n_color_a = &n_white;
        n_color_b = &n_black;
    }

    bool move_made = false;
    for (int8_t dr = -1; dr <= 1; dr++) {
        for (int8_t dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int8_t tr = row + dr;
            int8_t tc = col + dc;
            while (POS_IN_BOARD(tr, tc) && board_b[BOARD_INDEX(tr, tc)]) {
                tr += dr;
                tc += dc;
            }
            if (POS_IN_BOARD(tr, tc) && (tr != row + dr || tc != col + dc) &&
                board_a[BOARD_INDEX(tr, tc)]) {
                *n_color_a -= move_made;
                move_made = true;
                do {
                    tr -= dr;
                    tc -= dc;
                    board_a[BOARD_INDEX(tr, tc)] = TAKEN;
                    board_b[BOARD_INDEX(tr, tc)] = EMPTY;
                    (*n_color_a)++;
                    (*n_color_b)--;
                } while (tr != row || tc != col);
                (*n_color_b)++;
            }
        }
    }

    if (move_made) change_turn();
    return move_made;
}

/* othello - public class methods */
othello::othello() {
    black = (bool *)calloc(N * N, sizeof(bool));
    black[BOARD_INDEX(3, 4)] = TAKEN;
    black[BOARD_INDEX(4, 3)] = TAKEN;
    n_black = 2;

    white = (bool *)calloc(N * N, sizeof(bool));
    white[BOARD_INDEX(3, 3)] = TAKEN;
    white[BOARD_INDEX(4, 4)] = TAKEN;
    n_white = 2;

    turn = BLACK;
}

void othello::board_free() {
    free(black);
    free(white);
}

board_array *othello::next_boards() {
    board_array *next_boards = new board_array();
    next_boards->boards[0] = *this;
    for (int8_t r = 0; r < N; r++) {
        for (int8_t c = 0; c < N; c++) {
            if (!black[BOARD_INDEX(r, c)] && !white[BOARD_INDEX(r, c)] &&
                next_boards->boards[next_boards->n].make_move(r, c))
                next_boards->boards[++next_boards->n] = *this;
        }
    }

    next_boards->boards[next_boards->n].board_free();
    return next_boards;
}

void othello::print() {
    for (uint8_t r = 0; r < N; r++) {
        for (uint8_t c = 0; c < N; c++) {
            assert(!black[BOARD_INDEX(r, c)] || !white[BOARD_INDEX(r, c)]);
            char piece = black[BOARD_INDEX(r, c)]
                             ? 'B'
                             : (white[BOARD_INDEX(r, c)] ? 'W' : '.');
            std::cout << piece << ' ';
        }
        std::cout << std::endl;
    }

    std::cout << (int)n_black << ' ' << (int)n_white << std::endl;
    if (get_n_placed() == N * N)
        std::cout << "Game over" << std::endl;
    else
        std::cout << (turn == BLACK ? "BLACK" : "WHITE") << "'s turn"
                  << std::endl
                  << std::endl;
}

/* next_boards - public class methods */
board_array::board_array() {
    boards = (othello *)malloc(N * N * sizeof(othello));
    n = 0;
}

void board_array::array_free() {
    for (uint8_t i = 0; i < n; i++) boards[i].board_free();
    free(boards);
}
