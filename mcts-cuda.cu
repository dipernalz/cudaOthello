#include <cuda.h>
#include <curand.h>

#include "constants.hpp"
#include "mcts.hpp"
#include "othello.hpp"

#define B 8
#define T 8

extern curandGenerator_t rng;

static __device__ int is_move(int *black, int *white, int turn, int row,
                              int col) {
    int *board_a, *board_b;
    if (turn == BLACK) {
        board_a = black;
        board_b = white;
    } else {
        board_a = white;
        board_b = black;
    }

    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int tr = row + dr;
            int tc = col + dc;
            while (POS_IN_BOARD(tr, tc) && board_b[BOARD_INDEX(tr, tc)]) {
                tr += dr;
                tc += dc;
            }
            if (POS_IN_BOARD(tr, tc) && (tr != row + dr || tc != col + dc) &&
                board_a[BOARD_INDEX(tr, tc)])
                return true;
        }
    }

    return false;
}

static __device__ int generate_moves(int *black, int *white, int turn,
                                     int *moves_r, int *moves_c) {
    int n_moves = 0;
    for (int r = 0; r < N; r++) {
        for (int c = 0; c < N; c++) {
            if (!black[BOARD_INDEX(r, c)] && !white[BOARD_INDEX(r, c)] &&
                is_move(black, white, turn, r, c)) {
                moves_r[n_moves] = r;
                moves_c[n_moves] = c;
                n_moves++;
            }
        }
    }
    return n_moves;
}

static __device__ void make_move(int *black, int *white, int *turn,
                                 int *n_black, int *n_white, int row, int col) {
    int *board_a, *board_b;
    int *n_color_a, *n_color_b;
    if (*turn == BLACK) {
        board_a = black;
        board_b = white;
        n_color_a = n_black;
        n_color_b = n_white;
    } else {
        board_a = white;
        board_b = black;
        n_color_a = n_white;
        n_color_b = n_black;
    }

    int move_made = false;
    for (int dr = -1; dr <= 1; dr++) {
        for (int dc = -1; dc <= 1; dc++) {
            if (dr == 0 && dc == 0) continue;
            int tr = row + dr;
            int tc = col + dc;
            while (POS_IN_BOARD(tr, tc) && board_b[BOARD_INDEX(tr, tc)]) {
                tr += dr;
                tc += dc;
            }
            if (POS_IN_BOARD(tr, tc) && (tr != row + dr || tc != col + dc) &&
                board_a[BOARD_INDEX(tr, tc)]) {
                *n_color_a -= move_made;
                move_made = true;
                do {
                    tr -= dr;
                    tc -= dc;
                    board_a[BOARD_INDEX(tr, tc)] = TAKEN;
                    board_b[BOARD_INDEX(tr, tc)] = EMPTY;
                    (*n_color_a)++;
                    (*n_color_b)--;
                } while (tr != row || tc != col);
                (*n_color_b)++;
            }
        }
    }

    *turn = !*turn;
}

static __global__ void sim_rand_game(int *black, int *white, unsigned int *rand,
                                     int *turn, int *n_black, int *n_white,
                                     int *wins, int *losses) {
    __shared__ int s_black[T][N * N];
    __shared__ int s_white[T][N * N];
    __shared__ int s_moves_r[T][N * N];
    __shared__ int s_moves_c[T][N * N];

    int r_turn = *turn;
    int r_n_black = *n_black;
    int r_n_white = *n_white;
    int r_rand_idx = T * N * N * blockIdx.x + N * N * threadIdx.x;

    for (int i = 0; i < N * N; i++) {
        s_black[threadIdx.x][i] = black[i];
        s_white[threadIdx.x][i] = white[i];
    }

    int *piece_count_a, *piece_count_b;
    if (r_turn == BLACK) {
        piece_count_a = &r_n_black;
        piece_count_b = &r_n_white;
    } else {
        piece_count_a = &r_n_white;
        piece_count_b = &r_n_black;
    }

    int idx;
    while (r_n_black + r_n_white != N * N) {
        int n_moves =
            generate_moves(s_black[threadIdx.x], s_white[threadIdx.x], r_turn,
                           s_moves_r[threadIdx.x], s_moves_c[threadIdx.x]);
        if (n_moves == 0) {
            r_turn = !r_turn;
            n_moves = generate_moves(s_black[threadIdx.x], s_white[threadIdx.x],
                                     r_turn, s_moves_r[threadIdx.x],
                                     s_moves_c[threadIdx.x]);
            if (n_moves == 0) break;
            idx = rand[r_rand_idx++] % n_moves;
            make_move(s_black[threadIdx.x], s_white[threadIdx.x], &r_turn,
                      &r_n_black, &r_n_white, s_moves_r[threadIdx.x][idx],
                      s_moves_c[threadIdx.x][idx]);
            continue;
        }
        idx = rand[r_rand_idx++] % n_moves;
        make_move(s_black[threadIdx.x], s_white[threadIdx.x], &r_turn,
                  &r_n_black, &r_n_white, s_moves_r[threadIdx.x][idx],
                  s_moves_c[threadIdx.x][idx]);
    }

    atomicAdd(wins, *piece_count_a - *piece_count_b > 0 ? 1 : 0);
    atomicAdd(losses, *piece_count_a - *piece_count_b < 0 ? 1 : 0);
}

sim_results sim_games_cuda(othello *board) {
    sim_results results = {B * T, 0, 0};

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

    int *c_black, *c_white, *c_turn, *c_n_black, *c_n_white, *c_wins, *c_losses;
    unsigned int *c_rand;

    cudaMalloc(&c_black, N * N * sizeof(int));
    cudaMalloc(&c_white, N * N * sizeof(int));
    cudaMalloc(&c_rand, B * T * N * N * sizeof(unsigned int));
    cudaMalloc(&c_turn, sizeof(int));
    cudaMalloc(&c_n_black, sizeof(int));
    cudaMalloc(&c_n_white, sizeof(int));
    cudaMalloc(&c_wins, sizeof(int));
    cudaMalloc(&c_losses, sizeof(int));

    cudaMemcpy(c_black, h_black, N * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_white, h_white, N * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_turn, &h_turn, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_n_black, &h_n_black, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_n_white, &h_n_white, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemset(c_wins, 0, sizeof(int));
    cudaMemset(c_losses, 0, sizeof(int));

    curandGenerate(rng, c_rand, B * T * N * N);
    sim_rand_game<<<B, T>>>(c_black, c_white, c_rand, c_turn, c_n_black,
                            c_n_white, c_wins, c_losses);
    cudaMemcpy(&results.wins, c_wins, sizeof(int), cudaMemcpyDeviceToHost);
    cudaMemcpy(&results.losses, c_losses, sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(c_black);
    cudaFree(c_white);
    cudaFree(c_rand);
    cudaFree(c_turn);
    cudaFree(c_n_black);
    cudaFree(c_n_white);
    cudaFree(c_wins);
    cudaFree(c_losses);

    free(h_black);
    free(h_white);

    return results;
}
