/*
 * Copyright © 2019 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

/* Fast Monte-Carlo Bingo sim. */

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
    static char buf[5];
    char *colnames = "BINGO";
    // Numbers start at 1.
    int col = m / MARKER_ROWS;
    int w = snprintf(buf, sizeof buf, "%c%02u", colnames[col], m + 1);
    assert(w <= sizeof buf);
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
    if (!has_rdrand()) {
        fprintf(stderr, "program requires RDRAND CPU instruction: exiting\n");
        return 1;
    }
    // struct bitboard markers = bitboard_new();
    struct card card = make_card();
    print_card(&card);
    return 0;
}
