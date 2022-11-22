#include "mcts.hpp"

#include <iostream>

#include "constants.hpp"
#include "othello.hpp"
#include "vector.hpp"

class node {
   public:
    othello *board;
    node *parent;
    vector<node *> *children;
    uint32_t wins;
    uint32_t sims;

    node(othello *b) {
        board = b;
        parent = NULL;
        children = NULL;
        wins = 0;
        sims = 0;
    }

    ~node() {
        delete board;
        if (children != NULL) {
            for (uint8_t i = 0; i < children->size(); i++)
                delete children->get(i);
        }
        delete children;
    }

    void expand() {
        assert(children == NULL);
        children = new vector<node *>();
        vector<move> moves = board->generate_moves();
        for (uint8_t i = 0; i < moves.size(); i++) {
            othello *next_board = new othello(board);
            next_board->make_move(moves.get(i));
            children->push(new node(next_board));
        }
    }
};

int8_t sim_rand_game(othello *starting_board) {
    othello *board = new othello(starting_board);

    while (board->get_n_placed() != N * N) {
        vector<move> moves = board->generate_moves();
        if (moves.is_empty()) {
            board->change_turn();
            vector<move> moves = board->generate_moves();
            if (moves.is_empty()) break;
            board->make_move(moves.get_random_entry());
            continue;
        }
        board->make_move(moves.get_random_entry());
    }

    int8_t result =
        ((int8_t)board->get_n_black()) - ((int8_t)board->get_n_white());
    delete board;
    return result;
}

move find_best_move(othello *starting_board) {
    node root(new othello(starting_board));
    root.expand();
    return {0, 0};
}
