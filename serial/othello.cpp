#include "othello.hpp"

#include <cassert>
#include <cinttypes>
#include <iostream>

#include "constants.hpp"

#define BOARD_INDEX(r, c) (r * N + c)
#define POS_IN_BOARD(r, c) (r >= 0 && r < N && c >= 0 && c < N)

/* move_vector - public class methods */
move_vector::move_vector() {
    moves = new move[N * N];
    n = 0;
}

move_vector::~move_vector() { delete[] moves; }

/* othello - private class methods */
bool othello::is_move(uint8_t row, uint8_t col) {
    bool *board_a, *board_b;
    if (turn == BLACK) {
        board_a = black;
        board_b = white;
    } else {
        board_a = white;
        board_b = black;
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
                board_a[BOARD_INDEX(tr, tc)])
                return true;
        }
    }

    return false;
}

/* othello - public class methods */
othello::othello() {
    black = new bool[N * N]();
    black[BOARD_INDEX(3, 4)] = TAKEN;
    black[BOARD_INDEX(4, 3)] = TAKEN;
    n_black = 2;

    white = new bool[N * N]();
    white[BOARD_INDEX(3, 3)] = TAKEN;
    white[BOARD_INDEX(4, 4)] = TAKEN;
    n_white = 2;

    turn = BLACK;
}

othello::~othello() {
    delete[] black;
    delete[] white;
}

move_vector othello::generate_moves() {
    move_vector moves;
    for (uint8_t r = 0; r < N; r++) {
        for (uint8_t c = 0; c < N; c++) {
            if (!black[BOARD_INDEX(r, c)] && !white[BOARD_INDEX(r, c)] &&
                is_move(r, c))
                moves.add_move(r, c);
        }
    }
    return moves;
}

bool othello::make_move(move mv) {
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
            int8_t tr = mv.row + dr;
            int8_t tc = mv.col + dc;
            while (POS_IN_BOARD(tr, tc) && board_b[BOARD_INDEX(tr, tc)]) {
                tr += dr;
                tc += dc;
            }
            if (POS_IN_BOARD(tr, tc) &&
                (tr != mv.row + dr || tc != mv.col + dc) &&
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
                } while (tr != mv.row || tc != mv.col);
                (*n_color_b)++;
            }
        }
    }

    change_turn();
    return move_made;
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
