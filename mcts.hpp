#ifndef MCTS_H
#define MCTS_H

#include <cinttypes>

#include "othello.hpp"

typedef struct _sim_results {
    uint32_t n;
    uint32_t wins;
    uint32_t losses;
} sim_results;

void find_best_move(othello *starting_board, bool cuda);

#endif
