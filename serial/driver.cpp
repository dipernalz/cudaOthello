#include <iostream>

#include "constants.hpp"
#include "othello.hpp"

static void play_one_game(bool print) {
    othello board;
    while (true) {
        if (print) board.print();
        if (board.get_n_placed() == N * N) break;

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
}

static void play_n_games(uint32_t n, bool print) {
    for (uint32_t i = 0; i < n; i++) play_one_game(print);
}

int main() {
    srand((unsigned)time(NULL));

    // play_n_games(1, true);
    play_n_games(10000, false);
}
