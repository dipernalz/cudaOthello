#ifndef MCTS_CUDA_H
#define MCTS_CUDA_H

#include <cinttypes>

#include "mcts.hpp"
#include "othello.hpp"

sim_results sim_n_games_cuda(uint32_t n, othello *board);

#endif
