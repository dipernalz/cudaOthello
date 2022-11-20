#include "othello.hpp"

#include <inttypes.h>
#include <stdlib.h>

#include <iostream>

#define A 0
#define B 1
#define N 8

/* static helper functions */
static inline uint8_t get_index(uint8_t row, uint8_t col) {
    return N * row + col;
}

othello_board::othello_board() {
    black = (bool *)calloc(64, sizeof(bool));
    white = (bool *)calloc(64, sizeof(bool));
    black[get_index(4, 3)] = B;
    black[get_index(3, 4)] = B;
    white[get_index(3, 3)] = B;
    white[get_index(4, 4)] = B;
}

othello_board::~othello_board() {
    free(black);
    free(white);
}

void othello_board::print() {
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            char piece =
                (black[get_index(r, c)] ? 'B'
                                        : (white[get_index(r, c)] ? 'W' : '.'));
            std::cout << piece << ' ';
        }
        std::cout << std::endl;
    }
}
