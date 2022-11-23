#include <iostream>

#include "constants.hpp"
#include "mcts.hpp"
#include "othello.hpp"

int main() {
    srand((unsigned)time(NULL));

    uint32_t black_wins = 0, white_wins = 0, draws = 0;
    for (uint32_t i = 0; i < 100; i++) {
        othello *board = new othello();
        while (board->get_n_placed() != N * N) {
            move mv;
            if (board->get_turn() == BLACK) {
                mv = find_best_move(board);
            } else {
                vector<move> moves = board->generate_moves();
                if (moves.is_empty())
                    mv = {-1, -1};
                else
                    mv = moves.get_random_entry();
            }
            if (mv.row < 0) {
                board->change_turn();
                if (board->get_turn() == BLACK) {
                    mv = find_best_move(board);
                } else {
                    vector<move> moves = board->generate_moves();
                    if (moves.is_empty())
                        mv = {-1, -1};
                    else
                        mv = moves.get_random_entry();
                }
                if (mv.row < 0) break;
            }
            board->make_move(mv);
        }
        board->print();

        if (board->get_n_black() > board->get_n_white())
            black_wins++;
        else if (board->get_n_black() < board->get_n_white())
            white_wins++;
        else
            draws++;

        delete board;

        std::cout << "Black wins: " << black_wins << std::endl;
        std::cout << "White wins: " << white_wins << std::endl;
        std::cout << "Draws:      " << draws << std::endl << std::endl;
    }
}
