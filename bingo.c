/*
 * Fast Monte-Carlo Bingo Sim inspired by
 * https://www.reddit.com/r/rust/
 *    comments/agelwg/is_vecoptioni32_cachefriendly/ee6w7di/
 *
 * Written in C for my CS 201 course Winter 2019. Rust would
 * be a better choice otherwise.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "bitboard.h"
#include "rdrand.h"

#define BOARD_SIZE 5
#define MARKER_ROWS 15

typedef uint8_t board_t[BOARD_SIZE][BOARD_SIZE];

void make_board(board_t board) {
    for (int col = 0; col < BOARD_SIZE; col++) {
        int base = col * MARKER_ROWS;
        uint8_t markers[MARKER_ROWS];
        for (int m = 0; m < MARKER_ROWS; m++) {
            uint8_t marker = (uint8_t) (m + base);
            uint32_t s = randrange(m + 1);
            markers[m] = markers[s];
            markers[s] = marker;
        }
    }
}

int main() {
    struct bitboard markers = bitboard_new();
    
    return 0;
}
