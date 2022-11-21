#ifndef OTHELLO_H
#define OTHELLO_H

#include <cinttypes>
#include <cstdlib>
#include <cstring>

#include "constants.hpp"

class othello {
   private:
    bool *black;
    bool *white;
    bool turn;
    uint8_t n_black;
    uint8_t n_white;

    bool make_move(uint8_t row, uint8_t col);

   public:
    othello();

    othello &operator=(const othello &other) {
        black = (bool *)malloc(N * N * sizeof(bool));
        memcpy(black, other.black, N * N * sizeof(bool));

        white = (bool *)malloc(N * N * sizeof(bool));
        memcpy(white, other.white, N * N * sizeof(bool));

        turn = other.turn;
        n_black = other.n_black;
        n_white = other.n_white;

        return *this;
    }

    void board_free();

    othello *next_boards(uint8_t *n_found);

    inline void change_turn() { turn = !turn; }

    inline uint8_t get_n_placed() { return n_black + n_white; }

    void print();
};

#endif
