/* Representation of bingo markers as a bitboard */

#include <stdint.h>

struct bitboard {
    uint64_t bits[2];
};

extern struct bitboard bitboard_new();
extern void bitboard_setbit(struct bitboard *b, int bit);
extern int bitboard_hasbits(struct bitboard *b, struct bitboard *has);
