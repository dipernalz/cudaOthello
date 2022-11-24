CC = g++

CFLAGS = -I. -Wall -Werror -O3 -DNDEBUG=1
# CFLAGS = -I. -Wall -Werror -g

OBJ = othello.o mcts.o driver.o

othello: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

driver.o: driver.cpp
	$(CC) $(CFLAGS) -c -o $@ $<

%.o: %.cpp %.hpp vector.hpp constants.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f othello *.o
