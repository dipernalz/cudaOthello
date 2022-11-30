CC = nvcc

# CFLAGS = -I. -O3 -DNDEBUG=1
CFLAGS = -I. -g

OBJ = othello.o mcts.o mcts-cuda.o driver.o

othello: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.cpp %.hpp vector.hpp constants.hpp
	$(CC) $(CFLAGS) -c -o $@ $<

driver.o: driver.cu
	$(CC) $(CFLAGS) -c -o $@ $<

mcts-cuda.o: mcts-cuda.cu
	$(CC) $(CFLAGS) -c -o $@ $< 

clean:
	rm -f othello *.o
