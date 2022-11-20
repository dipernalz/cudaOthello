CC = g++

CFLAGS = -I. -Wall -Werror -O3 -DNDEBUG=1
# CFLAGS = -I. -Wall -Werror -g

DEPS = othello.hpp
OBJ = othello.o driver.o 

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

othello: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS)

clean:
	rm -f othello *.o
