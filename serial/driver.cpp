#include <cstdlib>
#include <iostream>

#include "constants.hpp"
#include "othello.hpp"

static void play_one_game(bool print) {
    othello board;
    while (true) {
        if (print) board.print();
        if (board.get_n_placed() == N * N) break;
        board_array *next_boards = board.next_boards();

        if (!next_boards->n) {
            next_boards->array_free();
            delete next_boards;
            board.change_turn();
            if (print) board.print();
            next_boards = board.next_boards();
            if (!next_boards->n) {
                next_boards->array_free();
                delete next_boards;
                break;
            }
        }

        board.board_free();
        board = next_boards->boards[rand() % next_boards->n];
        next_boards->array_free();
        delete next_boards;
    }
    board.board_free();
}

static void play_n_games(uint32_t n, bool print) {
    for (uint32_t i = 0; i < n; i++) play_one_game(print);
}

int main() {
    srand((unsigned)time(NULL));

    play_n_games(1, true);
    // play_n_games(10000, false);
}
