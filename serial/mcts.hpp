#ifndef MCTS_H
#define MCTS_H

#include <cinttypes>

#include "othello.hpp"

int8_t sim_rand_game(othello *starting_board);

move find_best_move(othello *starting_board);

#endif
