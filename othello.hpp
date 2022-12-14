#ifndef OTHELLO_H
#define OTHELLO_H

#include <cassert>
#include <cinttypes>
#include <cstdlib>

#include "constants.hpp"
#include "vector.hpp"

typedef struct _move {
    int8_t row;
    int8_t col;
} move;

class othello {
   private:
    bool is_move(uint8_t row, uint8_t col);

   public:
    bool *black;
    bool *white;
    bool turn;
    uint8_t n_black;
    uint8_t n_white;

    othello();

    othello(const othello *other);

    othello(char *input, char trn);

    ~othello();

    vector<move> *generate_moves();

    bool make_move(move m);

    int32_t eval();

    void print();

    void print_grid();

    inline void change_turn() { turn = !turn; }

    inline uint8_t get_n_placed() { return n_black + n_white; }
};

#endif
