#include <inttypes.h>

class othello_board {
   private:
    bool *black;
    bool *white;
    bool turn;

   public:
    othello_board();
    ~othello_board();
    othello_board **next_boards();
    void print();
};
