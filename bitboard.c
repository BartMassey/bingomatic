/*
 * Copyright © 2019 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

/* Bingo-specific implementation of bitboards. */

#include <stdint.h>

#include "bitboard.h"

struct bitboard bitboard_new() {
    struct bitboard b = {{0L, 0L}};
    return b;
}

void bitboard_setbit(struct bitboard *b, int bit) {
    int index;
    if (bit < 64) {
        index = 0;
    } else {
        index = 1;
        bit -= 64;
    }
    b->bits[index] |= 1L << bit;
}

int bitboard_hasbits(struct bitboard *b, struct bitboard *has) {
    for (int i = 0; i < 2; i++)
        if ((b->bits[i] & has->bits[i]) != has->bits[i])
            return 0;
    return 1;
}
