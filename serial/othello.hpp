#include <inttypes.h>

class othello_board {
   private:
    bool *black;
    bool *white;

   public:
    othello_board();
    ~othello_board();
    void print();
};
