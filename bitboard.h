/*
 * Copyright © 2019 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

/* Representation of bingo markers as a bitboard */

#include <stdint.h>

#include "bingo.h"

extern void bitboard_clear(struct markings *b);
extern void bitboard_set(struct markings *b, int bit);
extern int bitboard_mark(struct markings *b, int bit);
