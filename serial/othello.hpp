#ifndef OTHELLO_H
#define OTHELLO_H

#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "constants.hpp"

typedef struct _move {
    uint8_t row;
    uint8_t col;
} move;

class move_vector {
   private:
    move *moves;
    uint8_t n;

   public:
    move_vector();

    ~move_vector();

    inline void add_move(uint8_t &row, uint8_t &col) {
        moves[n++] = {row, col};
    }

    inline bool is_empty() { return !n; }

    inline move get_random_move() {
        assert(n != 0);
        return moves[rand() % n];
    }
};

class othello {
   private:
    bool *black;
    bool *white;
    bool turn;
    uint8_t n_black;
    uint8_t n_white;

    bool is_move(uint8_t row, uint8_t col);

   public:
    othello();

    othello(const othello &other);

    ~othello();

    move_vector generate_moves();

    bool make_move(move m);

    void print();

    inline void change_turn() { turn = !turn; }

    inline uint8_t &get_n_black() { return n_black; }

    inline uint8_t &get_n_white() { return n_white; }

    inline uint8_t get_n_placed() { return n_black + n_white; }
};

#endif
