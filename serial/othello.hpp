#include <cinttypes>
#include <cstdlib>
#include <cstring>

class othello {
   private:
    bool *black;
    bool *white;
    bool turn;

    bool make_move(uint8_t row, uint8_t col);

   public:
    othello();

    othello &operator=(const othello &other) {
        black = (bool *)malloc(64 * sizeof(bool));
        memcpy(black, other.black, 64 * sizeof(bool));

        white = (bool *)malloc(64 * sizeof(bool));
        memcpy(white, other.white, 64 * sizeof(bool));

        turn = other.turn;

        return *this;
    }

    void board_free();

    othello *next_boards(uint8_t *n_found);

    inline void change_turn() { turn = !turn; }

    void print();
};
