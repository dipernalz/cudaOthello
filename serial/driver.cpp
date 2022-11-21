#include <cstdlib>
#include <iostream>

#include "constants.hpp"
#include "othello.hpp"

static void free_board_array(othello *board_array, uint8_t n) {
    for (uint8_t i = 0; i < n; i++) board_array[i].board_free();
    free(board_array);
}

static void play_one_game(bool print) {
    othello board;
    while (true) {
        if (print) board.print();
        if (board.get_n_placed() == N * N) break;

        uint8_t n_found;
        othello *next_boards = board.next_boards(&n_found);

        if (!n_found) {
            free_board_array(next_boards, n_found);
            board.change_turn();
            if (print) board.print();

            next_boards = board.next_boards(&n_found);
            if (!n_found) {
                free_board_array(next_boards, n_found);
                board.board_free();
                break;
            }
        }

        board.board_free();
        board = next_boards[rand() % n_found];
        free_board_array(next_boards, n_found);
    }
}

static void play_n_games(uint32_t n, bool print) {
    for (uint32_t i = 0; i < n; i++) play_one_game(print);
}

int main() {
    srand((unsigned)time(NULL));

    play_n_games(1, true);
    // play_n_games(10000, false);
}
