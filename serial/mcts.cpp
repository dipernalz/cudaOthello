#include "mcts.hpp"

#include <cfloat>
#include <cmath>
#include <iostream>

#include "constants.hpp"
#include "othello.hpp"
#include "vector.hpp"

#define MAX_ITER 5000
#define N_SIMS 100

class node {
   public:
    othello *board;
    node *parent;
    vector<node *> *children;
    uint32_t wins;
    uint32_t n;
    move mv;

    node(othello *b) {
        board = b;
        parent = NULL;
        children = NULL;
        wins = 0;
        n = 0;
        mv = {0, 0};
    }

    ~node() {
        delete board;
        if (children != NULL) {
            for (uint8_t i = 0; i < children->size(); i++)
                delete children->get(i);
            delete children;
        }
    }

    void expand() {
        assert(children == NULL);
        children = new vector<node *>();
        vector<move> moves = board->generate_moves();
        for (uint8_t i = 0; i < moves.size(); i++) {
            othello *next_board = new othello(board);
            next_board->make_move(moves.get(i));
            node *child = new node(next_board);
            child->parent = this;
            child->mv = moves.get(i);
            children->push(child);
        }
    }

    node *select_child(uint32_t n, bool min) {
        assert(children != NULL);
        double best_uct = min ? DBL_MAX : DBL_MIN;
        node *best_child = NULL;
        for (uint8_t i = 0; i < children->size(); i++) {
            node *child = children->get(i);
            double uct = ((double)child->wins) / ((double)(child->n + 1)) +
                         2 * sqrt(log(n + 1) / ((double)(child->n + 1)));
            if (min) {
                if (uct < best_uct) {
                    best_uct = uct;
                    best_child = child;
                }
            } else {
                if (uct > best_uct) {
                    best_uct = uct;
                    best_child = child;
                }
            }
        }
        return best_child;
    }
};

typedef struct _sim_results {
    uint32_t n;
    uint32_t wins;
    uint32_t losses;
    uint32_t draws;
} sim_results;

static int8_t sim_rand_game(othello *starting_board) {
    othello *board = new othello(starting_board);

    uint8_t *piece_count_a, *piece_count_b;
    if (board->get_turn() == BLACK) {
        piece_count_a = &board->get_n_black();
        piece_count_b = &board->get_n_white();
    } else {
        piece_count_a = &board->get_n_white();
        piece_count_b = &board->get_n_black();
    }

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

    int8_t result = ((int8_t)*piece_count_a) - ((int8_t)*piece_count_b);
    delete board;
    return result;
}

static sim_results sim_n_games(uint32_t n, othello *board) {
    sim_results results = {n, 0, 0, 0};
    for (uint32_t i = 0; i < n; i++) {
        int8_t result = sim_rand_game(board);
        if (result > 0)
            results.wins++;
        else if (result < 0)
            results.losses++;
        else
            results.draws++;
    }
    return results;
}

static void backprop(sim_results &results, node *&nd) {
    node *current_node = nd;
    while (current_node != NULL) {
        current_node->n += results.n;
        current_node->wins +=
            current_node->board->get_turn() == nd->board->get_turn()
                ? results.wins
                : results.losses;
        current_node = current_node->parent;
    }
}

void find_best_move(othello *starting_board) {
    if (starting_board->generate_moves().is_empty()) return;

    node *root = new node(new othello(starting_board));

    uint32_t n = 0;
    for (uint32_t i = 0; i < MAX_ITER; i++) {
        node *current_node = root;
        while (current_node->children && !current_node->children->is_empty())
            current_node = current_node->select_child(n, false);
        if (current_node->n != 0 && current_node->children == NULL) {
            current_node->expand();
            if (!current_node->children->is_empty())
                current_node = current_node->select_child(n, false);
        }
        sim_results results = sim_n_games(N_SIMS, current_node->board);
        backprop(results, current_node);
        n += N_SIMS;

        if (root->children != NULL) {
            othello *board = new othello(starting_board);
            board->make_move(root->select_child(0, true)->mv);
            board->print();
            delete board;
        }
    }

    delete root;
}
