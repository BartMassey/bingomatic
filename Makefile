CC = gcc
CFLAGS = -O4 -Wall

bingo: bingo.o bitboard.o
	$(CC) $(CFLAGS) -o bingo $?

testrdrand: testrdrand.c
	$(CC) $(CFLAGS) -o testrdrand testrdrand.c

bingo.o bitboard.o: bitboard.h

bingo.o testrdrand: rdrand.h

clean:
	-rm -f *.o bingo testrdrand
