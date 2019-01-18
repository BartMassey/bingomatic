/*
 * Copyright © 2019 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

/* Representation of bingo markers as a bitboard */

#include <stdint.h>

struct bitboard {
    uint64_t bits[2];
};

extern struct bitboard bitboard_new();
extern void bitboard_setbit(struct bitboard *b, int bit);
extern int bitboard_hasbits(struct bitboard *b, struct bitboard *has);
