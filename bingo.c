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

#define CARD_SIZE 5
#define MARKER_ROWS 15

struct card {
    uint8_t squares[CARD_SIZE][CARD_SIZE];
};

struct card make_card() {
    struct card card;
    for (int col = 0; col < CARD_SIZE; col++) {
        int base = col * MARKER_ROWS;
        uint8_t markers[MARKER_ROWS];
        for (int m = 0; m < MARKER_ROWS; m++) {
            uint8_t marker = (uint8_t) (m + base);
            uint32_t s = randrange(m + 1);
            markers[m] = markers[s];
            markers[s] = marker;
        }
        for (int row = 0; row < CARD_SIZE; row++)
            card.squares[row][col] = markers[row];
    }
    return card;
}

/*
 * XXX This function returns a pointer to static data whose
 * contents are overwritten on each call.
 */
char *marker_string(uint8_t m) {
    assert(CARD_SIZE == 5);
    static char buf[4];
    char *colnames = "BINGO";
    int col = m / MARKER_ROWS;
    int row = m % MARKER_ROWS;
    snprintf(buf, sizeof buf, "%c%02d", colnames[col], row);
    return buf;
}

void print_card(struct card *card) {
    for (int row = 0; row < CARD_SIZE; row++) {
        char *sep = "";
        for (int col = 0; col < CARD_SIZE; col++) {
            printf("%s%s", sep, marker_string(card->squares[row][col]));
            sep = " ";
        }
        printf("\n");
    }
}

int main() {
    assert(has_rdrand());
    // struct bitboard markers = bitboard_new();
    struct card card = make_card();
    print_card(&card);
    return 0;
}
