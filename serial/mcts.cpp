#include "mcts.hpp"

#include <iostream>

#include "constants.hpp"
#include "othello.hpp"

int8_t sim_rand_game(othello &starting_board) {
    othello board = starting_board;

    while (board.get_n_placed() != N * N) {
        move_vector moves = board.generate_moves();
        if (moves.is_empty()) {
            board.change_turn();
            move_vector moves = board.generate_moves();
            if (moves.is_empty()) break;
            board.make_move(moves.get_random_move());
            continue;
        }
        board.make_move(moves.get_random_move());
    }

    return ((int8_t)board.get_n_black()) - ((int8_t)board.get_n_white());
}
