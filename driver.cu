#include <cuda.h>
#include <curand.h>
#include <unistd.h>

#include <iostream>

#include "constants.hpp"
#include "mcts.hpp"
#include "othello.hpp"

curandGenerator_t rng;

int main(int ac, char **av) {
    srand((unsigned)time(NULL) ^ getpid());

    othello *board = new othello(av[1], *av[2]);

    switch (*av[3]) {
        // random move
        case '0': {
            vector<move> *moves = board->generate_moves();
            if (!moves->is_empty()) {
                board->make_move(moves->get_random_entry());
                board->print();
            }
            delete moves;
            break;
        }
        // MCTS move
        case '1': {
            find_best_move(board, false);
            break;
        }
        // make given move
        case '2': {
            board->make_move({(int8_t)atoi(av[4]), (int8_t)atoi(av[5])});
            board->print();
        }
        // MCTS with CUDA move
        case '3': {
            cudaSetDevice(2);
            curandCreateGenerator(&rng, CURAND_RNG_PSEUDO_MTGP32);
            curandSetPseudoRandomGeneratorSeed(rng,
                                               (unsigned)time(NULL) ^ getpid());
            find_best_move(board, true);
            curandDestroyGenerator(rng);
            break;
        }
    }

    delete board;
}
