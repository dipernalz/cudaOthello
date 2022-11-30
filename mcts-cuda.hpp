#ifndef MCTS_CUDA_H
#define MCTS_CUDA_H

#include <cinttypes>

#include "mcts.hpp"
#include "othello.hpp"

sim_results sim_games_cuda(othello *board);

#endif
