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

    // #pragma unroll
    for (int dr = -1; dr <= 1; dr++) {
        // #pragma unroll
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
    // #pragma unroll
    for (int r = 0; r < N; r++) {
        // #pragma unroll
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
    // #pragma unroll
    for (int dr = -1; dr <= 1; dr++) {
        // #pragma unroll
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

static __global__ void sim_rand_game(int *black, int *white, int *turn,
                                     int *n_black, int *n_white, int *result) {
    __shared__ int s_black[64];
    __shared__ int s_white[64];
    __shared__ int s_moves_r[64];
    __shared__ int s_moves_c[64];

    int r_turn = *turn;
    int r_n_black = *n_black;
    int r_n_white = *n_white;

#pragma unroll
    for (int i = 0; i < N * N; i++) {
        s_black[i] = black[i];
        s_white[i] = white[i];
    }

    int *piece_count_a, *piece_count_b;
    if (r_turn == BLACK) {
        piece_count_a = &r_n_black;
        piece_count_b = &r_n_white;
    } else {
        piece_count_a = &r_n_white;
        piece_count_b = &r_n_black;
    }

    while (r_n_black + r_n_white != N * N) {
        int n_moves =
            generate_moves(s_black, s_white, r_turn, s_moves_r, s_moves_c);
        if (n_moves == 0) {
            r_turn = !r_turn;
            n_moves =
                generate_moves(s_black, s_white, r_turn, s_moves_r, s_moves_c);
            if (n_moves == 0) break;
            make_move(s_black, s_white, &r_turn, &r_n_black, &r_n_white,
                      s_moves_r[0], s_moves_c[0]);
            continue;
        }
        make_move(s_black, s_white, &r_turn, &r_n_black, &r_n_white,
                  s_moves_r[0], s_moves_c[0]);
    }

    *result = *piece_count_a - *piece_count_b;
}

void reset_cuda_board(int *h_black, int *h_white, int *h_turn, int *h_n_black,
                      int *h_n_white, int *c_black, int *c_white, int *c_turn,
                      int *c_n_black, int *c_n_white) {
    cudaMemcpy(c_black, h_black, N * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_white, h_white, N * N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_turn, h_turn, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_n_black, h_n_black, sizeof(int), cudaMemcpyHostToDevice);
    cudaMemcpy(c_n_white, h_n_white, sizeof(int), cudaMemcpyHostToDevice);
}

sim_results sim_n_games_cuda(uint32_t n, othello *board) {
    sim_results results = {n, 0, 0};

    int *h_black, *h_white, h_turn, h_n_black, h_n_white, h_result;
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
        cudaMemcpy(&h_result, c_result, sizeof(int), cudaMemcpyDeviceToHost);
        if (h_result > 0)
            results.wins++;
        else if (h_result < 0)
            results.losses++;
        break;
    }

    cudaFree(c_black);
    cudaFree(c_white);
    cudaFree(c_turn);
    cudaFree(c_n_black);
    cudaFree(c_n_white);
    cudaFree(c_result);

    return results;
}
