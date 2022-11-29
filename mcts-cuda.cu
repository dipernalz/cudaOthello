#include <cuda.h>

#include "constants.hpp"
#include "mcts.hpp"
#include "othello.hpp"

move choose_move(othello *board, vector<move> *&moves) {
    for (uint8_t i = 0; i < moves->size(); i++) {
        move mv = moves->get(i);
        if ((mv.row == 0 || mv.row == N - 1) &&
            (mv.col == 0 || mv.col == N - 1)) {
            return mv;
        }
    }

    move mv = moves->get(rand() % moves->size());
    for (uint8_t i = 0; i < 4; i++) {
        if ((mv.row == 1 || mv.row == N - 2) &&
            (mv.col == 1 || mv.col == N - 2))
            mv = moves->get(rand() % moves->size());
        else
            break;
    }
    return mv;
}

__global__ void sim_rand_game(int *black, int *white, int *turn, int *n_black,
                              int *n_white, int *result) {
    __shared__ int s_black[64];
    __shared__ int s_white[64];
    int r_turn = *turn;
    int r_n_black = *n_black;
    int r_n_white = *n_white;

#pragma unroll
    for (int i = 0; i < N * N; i++) {
        s_black[i] = black[i];
        s_white[i] = white[i];
    }

    printf("cuda\n");
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            printf("%c ", s_black[BOARD_INDEX(r, c)]
                              ? 'B'
                              : (s_white[BOARD_INDEX(r, c)] ? 'W' : '.'));
        }
        printf("\n");
    }

    // othello *board = new othello(starting_board);

    // uint8_t *piece_count_a, *piece_count_b;
    // if (board->get_turn() == BLACK) {
    //     piece_count_a = &board->get_n_black();
    //     piece_count_b = &board->get_n_white();
    // } else {
    //     piece_count_a = &board->get_n_white();
    //     piece_count_b = &board->get_n_black();
    // }

    // while (board->get_n_placed() != N * N) {
    //     vector<move> *moves = board->generate_moves();
    //     if (moves->is_empty()) {
    //         board->change_turn();
    //         delete moves;
    //         vector<move> *moves = board->generate_moves();
    //         if (moves->is_empty()) {
    //             delete moves;
    //             break;
    //         }
    //         board->make_move(choose_move(board, moves));
    //         delete moves;
    //         continue;
    //     }
    //     board->make_move(choose_move(board, moves));
    //     delete moves;
    // }

    // int8_t result = ((int8_t)*piece_count_a) - ((int8_t)*piece_count_b);
    // delete board;
    // return result;
}

void reset_cuda_board(int *h_black, int *h_white, int *h_turn, int *h_n_black,
                      int *h_n_white, int *c_black, int *c_white, int *c_turn,
                      int *c_n_black, int *c_n_white) {
    cudaMemcpy(c_black, h_black, N * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_white, h_white, N * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_turn, h_turn, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_n_black, h_n_black, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_n_black, h_n_black, sizeof(int), cudaMemcpyHostToDevice);
}

sim_results sim_n_games_cuda(uint32_t n, othello *board) {
    sim_results results = {n, 0, 0};

    int *h_black, *h_white, h_turn, h_n_black, h_n_white;
    h_black = (int *)malloc(N * N * sizeof(int));
    h_white = (int *)malloc(N * N * sizeof(int));
    for (int i = 0; i < N * N; i++) {
        h_black[i] = (int)board->black[i];
        h_white[i] = (int)board->white[i];
        h_turn = (int)board->turn;
        h_n_black = (int)board->n_black;
        h_n_white = (int)board->n_white;
    }

    int *c_black, *c_white, *c_turn, *c_n_black, *c_n_white, *c_result;
    cudaMalloc(&c_black, N * N * sizeof(int));
    cudaMalloc(&c_white, N * N * sizeof(int));
    cudaMalloc(&c_turn, sizeof(int));
    cudaMalloc(&c_n_black, sizeof(int));
    cudaMalloc(&c_n_white, sizeof(int));
    cudaMalloc(&c_result, sizeof(int));

    for (uint32_t i = 0; i < n; i++) {
        reset_cuda_board(h_black, h_white, &h_turn, &h_n_black, &h_n_white,
                         c_black, c_white, c_turn, c_n_black, c_n_white);
        sim_rand_game<<<1, 1>>>(c_black, c_white, c_turn, c_n_black, c_n_white,
                                c_result);
        // int8_t result = sim_rand_game(board);
        // if (result > 0)
        //     results.wins++;
        // else if (result < 0)
        //     results.losses++;
    }

    cudaFree(c_black);
    cudaFree(c_white);
    cudaFree(c_turn);
    cudaFree(c_n_black);
    cudaFree(c_n_white);

    return results;
}
