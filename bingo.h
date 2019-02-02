/*
 * Copyright © 2019 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

/* Bingo shared state */

#define CARD_SIZE 5

struct markings {
    uint8_t increments[CARD_SIZE * CARD_SIZE];
    uint8_t d_counters;
    uint16_t markers_high;
    uint32_t rc_counters;
    uint64_t markers_low;
};

static inline void bitboard_clear(struct markings *b) {
    b->markers_high = 0;
    b->markers_low = 0;
}

static inline void bitboard_set(struct markings *b, int bit) {
    assert(sizeof b->markers_low == 8);
    assert(sizeof b->markers_high == 2);
    if (bit < 64) {
        uint64_t low_posn = 1L << bit;
        b->markers_low |= low_posn;
        return;
    }
    uint16_t high_posn = 1 << (bit - 64);
    b->markers_high |= high_posn;
}

/*
 * Returns -1 if the given bit isn't set in the markings bitboard.
 * Otherwise returns the index into the markings instructions (0..24).
 */
static inline int bitboard_mark(struct markings *b, int bit) {
    assert(sizeof b->markers_low == 8);
    assert(sizeof b->markers_high == 2);
    uint64_t markers_low = b->markers_low;
    if (bit < 64) {
        uint64_t low_posn = 1L << bit;
        if (!(markers_low & low_posn))
            return -1;
        uint64_t low_bits = markers_low & (low_posn - 1);
        return __builtin_popcountl(low_bits);
    }
    uint16_t high_posn = 1 << (bit - 64);
    uint16_t markers_high = b->markers_high;
    if (!(markers_high & high_posn))
        return -1;
    int low_count = __builtin_popcountl(markers_low);
    uint16_t high_bits = markers_high & (high_posn - 1);
    return low_count + __builtin_popcount(high_bits);
}
