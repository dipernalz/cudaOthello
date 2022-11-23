#include <unistd.h>

#include <iostream>

#include "constants.hpp"
#include "mcts.hpp"
#include "othello.hpp"

int main(int ac, char **av) {
    srand((unsigned)time(NULL) ^ getpid());

    othello *board = new othello(av[1], *av[2]);

    switch (*av[3]) {
        // random move
        case '0': {
            vector<move> moves = board->generate_moves();
            if (!moves.is_empty()) {
                board->make_move(moves.get_random_entry());
                board->print();
            }
            break;
        }
        // MCTS move
        case '1': {
            find_best_move(board);
            break;
        }
        // make given move
        case '2': {
            board->make_move({(int8_t)atoi(av[4]), (int8_t)atoi(av[5])});
            board->print();
        }
    }

    delete board;
}
