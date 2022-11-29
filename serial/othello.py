#!/usr/bin/env python3

import subprocess


N = 8
TIMEOUT = 5
STARTING_BOARD = "...........................WB......BW..........................."


def other_turn(turn):
    return "B" if turn == "W" else "W"


def print_board(board):
    for r in range(N):
        for c in range(N):
            print(f"{board[r * N + c]} ", end="")
        print()


def get_next_board(board, turn, mode, r=None, c=None):
    try:
        output = subprocess.run(
            ["./othello", board, turn, mode, str(r), str(c)],
            timeout=TIMEOUT,
            capture_output=True,
        ).stdout
    except subprocess.TimeoutExpired as e:
        output = e.stdout
    board = output.decode("UTF-8").strip().split('\n')[-1]
    return board if board else None


def play_n_games(n):
    global TIMEOUT
    TIMEOUT = .1
    wins = 0
    draws = 0
    losses = 0
    for i in range(n):
        board = STARTING_BOARD
        start_turn = "W" if i % 2 else "B"
        turn = "B"
        while board.count("."):
            next_board = get_next_board(
                board, turn, "1" if turn == start_turn else "0",
            )
            if not next_board:
                turn = other_turn(turn) 
                next_board = get_next_board(
                    board, turn, "1" if turn == start_turn else "0",
                )
                if not next_board:
                    break
            board = next_board
            turn = other_turn(turn)
            
        if board.count(start_turn) > board.count(other_turn(start_turn)):
            wins += 1
        elif board.count(start_turn) == board.count(other_turn(start_turn)):
            draws += 1
        else:
            losses += 1

        print_board(board)
        print(board.count("B"), board.count("W"))
        print(f"+{wins}-{losses}={draws}")
        print()


def play_user(start_turn):
    board = STARTING_BOARD
    turn = "B"
    while board.count("."):
        print_board(board)
        print()
        if turn == start_turn:
            r, c = map(int, input("Enter a move (r c): ").strip().split())
            print()
            next_board = get_next_board(board, turn, "2", r, c)
        else:
            print("Waiting...\n")
            next_board = get_next_board(board, turn, "1")
        if not next_board:
            turn = other_turn(turn)
            if turn == start_turn:
                r, c = map(int, input("Enter a move (r c): ").strip().split())
                print()
                next_board = get_next_board(board, turn, "2", r, c)
            else:
                next_board = get_next_board(board, turn, "1")
            if not next_board:
                return
        board = next_board
        turn = other_turn(turn)
    print_board(board)
    print(board.count("B"), board.count("W"))


def main():
    # play_n_games(1000)
    play_user("W")


if __name__ == "__main__":
    main()
