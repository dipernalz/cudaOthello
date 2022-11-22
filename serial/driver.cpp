#include <iostream>

#include "constants.hpp"
#include "mcts.hpp"
#include "othello.hpp"

int main() {
    srand((unsigned)time(NULL));

    othello *board = new othello();
    find_best_move(board);
    delete board;

    // othello *board = new othello();
    // uint32_t black_wins = 0;
    // uint32_t white_wins = 0;
    // uint32_t draws = 0;

    // for (int i = 0; i < 10000; i++) {
    //     int8_t result = sim_rand_game(board);
    //     if (result > 0)
    //         black_wins++;
    //     else if (result < 0)
    //         white_wins++;
    //     else
    //         draws++;
    // }

    // delete board;

    // std::cout << "Black wins: " << black_wins << std::endl;
    // std::cout << "White wins: " << white_wins << std::endl;
    // std::cout << "Draws:      " << draws << std::endl;
}
