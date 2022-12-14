#include "mcts.hpp"

#include <cfloat>
#include <cmath>
#include <iostream>

#include "constants.hpp"
#include "mcts-cuda.hpp"
#include "othello.hpp"
#include "vector.hpp"

#define MAX_ITER 100000
#define N_SIMS 64

class node {
   public:
    othello *board;
    move mv;

    node *parent;
    vector<node *> *children;
    vector<move> *unvisited;

    uint32_t wins;
    uint32_t n;

    node(othello *b, node *p) {
        board = b;

        parent = p;
        children = new vector<node *>();
        unvisited = board->generate_moves();

        wins = 0;
        n = 0;
    }

    ~node() {
        delete board;
        for (uint8_t i = 0; i < children->size(); i++) delete children->get(i);
        delete children;
        delete unvisited;
    }

    node *expand() {
        move mv = unvisited->remove(rand() % unvisited->size());
        othello *next_board = new othello(board);
        next_board->make_move(mv);

        node *child = new node(next_board, this);
        child->mv = mv;
        children->push(child);
        return child;
    }

    node *select_child(uint32_t n) {
        assert(children != NULL);
        double best_score = DBL_MIN;
        node *best_child = NULL;
        for (uint8_t i = 0; i < children->size(); i++) {
            node *child = children->get(i);
            double uct = ((double)child->wins) / ((double)child->n) +
                         sqrt(2. * log(n) / ((double)child->n));
            double score = uct;
            if ((child->mv.row == 0 || child->mv.row == N - 1) &&
                (child->mv.col == 0 || child->mv.col == N - 1)) {
                score *= 1.5;
            }
            if ((child->mv.row == 1 || child->mv.row == N - 2) &&
                (child->mv.col == 1 || child->mv.col == N - 2)) {
                score /= 1.5;
            }
            if (score > best_score) {
                best_score = score;
                best_child = child;
            }
        }
        return best_child;
    }
};

static move choose_move(othello *board, vector<move> *&moves) {
    return moves->get(rand() % moves->size());
}

static int8_t sim_rand_game(othello *starting_board) {
    othello *board = new othello(starting_board);

    uint8_t *piece_count_a, *piece_count_b;
    if (board->turn == BLACK) {
        piece_count_a = &board->n_black;
        piece_count_b = &board->n_white;
    } else {
        piece_count_a = &board->n_white;
        piece_count_b = &board->n_black;
    }

    while (board->get_n_placed() != N * N) {
        vector<move> *moves = board->generate_moves();
        if (moves->is_empty()) {
            board->change_turn();
            delete moves;
            vector<move> *moves = board->generate_moves();
            if (moves->is_empty()) {
                delete moves;
                break;
            }
            board->make_move(choose_move(board, moves));
            delete moves;
            continue;
        }
        board->make_move(choose_move(board, moves));
        delete moves;
    }

    int8_t result = ((int8_t)*piece_count_a) - ((int8_t)*piece_count_b);
    delete board;
    return result;
}

static sim_results sim_n_games(uint32_t n, othello *board) {
    sim_results results = {n, 0, 0};
    for (uint32_t i = 0; i < n; i++) {
        int8_t result = sim_rand_game(board);
        if (result > 0)
            results.wins++;
        else if (result < 0)
            results.losses++;
    }
    return results;
}

static void backprop(sim_results &results, node *&nd) {
    node *current_node = nd;
    while (current_node != NULL) {
        current_node->n += results.n;
        current_node->wins += current_node->board->turn == nd->board->turn
                                  ? results.wins
                                  : results.losses;
        current_node = current_node->parent;
    }
}

void find_best_move(othello *starting_board, bool cuda) {
    vector<move> *moves = starting_board->generate_moves();
    if (moves->is_empty()) {
        delete moves;
        return;
    }
    delete moves;

    node *root = new node(new othello(starting_board), NULL);

    uint32_t n = 0;
    for (uint32_t i = 0; i < MAX_ITER; i++) {
        node *current_node = root;
        while (current_node->unvisited->size() == 0 &&
               current_node->children->size() > 0)
            current_node = current_node->select_child(n);
        if (current_node->unvisited->size() != 0)
            current_node = current_node->expand();
        sim_results results;
        if (cuda)
            results = sim_games_cuda(current_node->board);
        else
            results = sim_n_games(N_SIMS, current_node->board);
        backprop(results, current_node);
        n += results.n;

        if (root->children != NULL) {
            move mv;
            double best_score = 0;
            for (int i = 0; i < root->children->size(); i++) {
                node *child = root->children->get(i);
                double score = ((double)child->n) / ((double)(child->wins + 1));
                if (starting_board->get_n_placed() <= 48) {
                    if ((child->mv.row == 0 || child->mv.row == N - 1) &&
                        (child->mv.col == 0 || child->mv.col == N - 1)) {
                        score *= 1.5;
                    }
                    if ((child->mv.row == 1 || child->mv.row == N - 2) &&
                        (child->mv.col == 1 || child->mv.col == N - 2)) {
                        score /= 1.5;
                    }
                }
                if (score > best_score) {
                    best_score = score;
                    mv = child->mv;
                }
            }
            othello *board = new othello(starting_board);
            board->make_move(mv);
            board->print();
            delete board;
        }
    }

    delete root;
}
