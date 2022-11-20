#include <stdlib.h>

#include <iostream>

#include "othello.hpp"

int main() {
    othello_board board;
    board.print();

    uint8_t n_found;
    othello_board *next_boards = board.next_boards(&n_found);
    for (int i = 0; i < n_found; i++) {
        std::cout << std::endl;
        next_boards[i].print();
    }

    free(next_boards);
}
