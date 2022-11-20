#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

class othello_board {
   private:
    bool *black;
    bool *white;
    bool turn;

   public:
    othello_board();

    ~othello_board();

    othello_board &operator=(const othello_board &other) {
        black = (bool *)malloc(64 * sizeof(bool));
        memcpy(black, other.black, 64 * sizeof(bool));

        white = (bool *)malloc(64 * sizeof(bool));
        memcpy(white, other.white, 64 * sizeof(bool));

        turn = other.turn;

        return *this;
    }

    othello_board *next_boards(uint8_t *n_found);

    void print();
};
