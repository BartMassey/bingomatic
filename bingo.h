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
