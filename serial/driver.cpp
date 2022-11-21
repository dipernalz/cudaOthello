#include <cstdlib>
#include <iostream>

#include "othello.hpp"

static void free_board_array(othello *board_array, uint8_t n) {
    for (uint8_t i = 0; i < n; i++) board_array[i].board_free();
    free(board_array);
}

int main() {
    othello board;
    while (true) {
        board.print();

        uint8_t n_found;
        othello *next_boards = board.next_boards(&n_found);

        if (!n_found) {
            free_board_array(next_boards, n_found);
            board.change_turn();
            board.print();

            next_boards = board.next_boards(&n_found);
            if (!n_found) {
                free_board_array(next_boards, n_found);
                board.board_free();
                break;
            }
        }

        board.board_free();
        board = next_boards[0];
        free_board_array(next_boards, n_found);
    }
}
