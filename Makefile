# Copyright © 2019 Bart Massey
# [This program is licensed under the "MIT License"]
# Please see the file LICENSE in the source
# distribution of this software for license terms.

CC = gcc
#CDEBUG = -g -DLOGGING
CDEBUG = -O4
INCLUDES = -I/usr/local/include
LIBS = -L/usr/local/lib -ltoyrand
DEFINES = # -DRECARD
CFLAGS = $(CDEBUG) -Wall $(INCLUDES) $(DEFINES)

bingo: bingo.o bitboard.o
	$(CC) $(CFLAGS) -o bingo bingo.o bitboard.o $(LIBS)

bingo.o bitboard.o: bingo.h

bitboard.o: popcnt.h

clean:
	-rm -f *.o bingo
