# Copyright © 2019 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file LICENSE in the source
# distribution of this software for license terms.

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
