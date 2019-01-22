# Copyright © 2019 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file LICENSE in the source
# distribution of this software for license terms.

CC = gcc
#CDEBUG = -g -DLOGGING
CDEBUG = -O4
INCLUDES = -I/usr/local/include
LIBS = -L/usr/local/lib -ltoyrand
CFLAGS = $(CDEBUG) -Wall $(INCLUDES)

bingo: bingo.o bitboard.o
	$(CC) $(CFLAGS) -o bingo bingo.o bitboard.o $(LIBS)

bingo.o bitboard.o: bitboard.h

clean:
	-rm -f *.o bingo
