#include "mcts.hpp"

#include <iostream>

#include "constants.hpp"
#include "othello.hpp"
#include "vector.hpp"

int8_t sim_rand_game(othello &starting_board) {
    othello board = starting_board;

    while (board.get_n_placed() != N * N) {
        vector<move> moves = board.generate_moves();
        if (moves.is_empty()) {
            board.change_turn();
            vector<move> moves = board.generate_moves();
            if (moves.is_empty()) break;
            board.make_move(moves.get_random_entry());
            continue;
        }
        board.make_move(moves.get_random_entry());
    }

    return ((int8_t)board.get_n_black()) - ((int8_t)board.get_n_white());
}
