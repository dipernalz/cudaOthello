#include "othello.hpp"

#include <cassert>
#include <cinttypes>
#include <cstring>
#include <iostream>

#include "constants.hpp"
#include "vector.hpp"

#define BOARD_INDEX(r, c) (r * N + c)
#define POS_IN_BOARD(r, c) (r >= 0 && r < N && c >= 0 && c < N)

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

othello::othello(const othello *other) {
    black = new bool[N * N];
    memcpy(black, other->black, N * N * sizeof(bool));

    white = new bool[N * N];
    memcpy(white, other->white, N * N * sizeof(bool));

    turn = other->turn;
    n_black = other->n_black;
    n_white = other->n_white;
}

othello::othello(char *input, char trn) {
    black = new bool[N * N]();
    white = new bool[N * N]();

    for (int i = 0; i < N * N; i++) {
        if (input[i] == 'B') {
            black[i] = TAKEN;
            n_black++;
        } else if (input[i] == 'W') {
            white[i] = TAKEN;
            n_white++;
        }
    }

    turn = trn == 'B' ? BLACK : WHITE;
}

othello::~othello() {
    delete[] black;
    delete[] white;
}

vector<move> othello::generate_moves() {
    vector<move> moves;
    for (int8_t r = 0; r < N; r++) {
        for (int8_t c = 0; c < N; c++) {
            if (!black[BOARD_INDEX(r, c)] && !white[BOARD_INDEX(r, c)] &&
                is_move(r, c))
                moves.push({r, c});
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

int32_t othello::eval() {
    bool *board_a, *board_b;
    if (turn == BLACK) {
        board_a = black;
        board_b = white;
    } else {
        board_a = white;
        board_b = black;
    }

    int32_t score = 0;

    uint8_t x[2] = {0, N - 1};
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 2; j++) {
            if (board_a[BOARD_INDEX(x[i], x[j])]) score += 10;
            if (board_b[BOARD_INDEX(x[i], x[j])]) score -= 10;
        }
    }
    uint8_t y[2] = {1, N - 2};
    for (uint8_t i = 0; i < 2; i++) {
        for (uint8_t j = 0; j < 2; j++) {
            if (board_a[BOARD_INDEX(y[i], y[j])]) score -= 5;
            if (board_b[BOARD_INDEX(y[i], y[j])]) score += 5;
        }
    }

    score += this->generate_moves().size();
    change_turn();
    score -= this->generate_moves().size();
    change_turn();

    return score;
}

void othello::print() {
    for (uint8_t r = 0; r < N; r++) {
        for (uint8_t c = 0; c < N; c++) {
            assert(!black[BOARD_INDEX(r, c)] || !white[BOARD_INDEX(r, c)]);
            char piece = black[BOARD_INDEX(r, c)]
                             ? 'B'
                             : (white[BOARD_INDEX(r, c)] ? 'W' : '.');
            std::cout << piece;
        }
    }
    std::cout << std::endl;
}

void othello::print_grid() {
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
