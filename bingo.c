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
#define NMARKERS (CARD_SIZE * MARKER_ROWS)

/*
 * Storage order for bingo bitboards:
 *   Row 0, 1, 2
 *   Col 0, 1, 2
 *   "Negative Diagonal" (0,0), (1,1) .. (4,4)
 *   "Positive Diagonal" (4,0), (3,1) .. (0,4)
 */
enum bingos {
    BINGO_ROWS = CARD_SIZE,
    BINGO_COLS = BINGO_ROWS + CARD_SIZE,
    BINGO_TOTAL = BINGO_COLS + 2,
};

enum win_class {
    WIN_ROW,
    WIN_COL,
    WIN_DIAG,
};

struct card {
    uint8_t squares[CARD_SIZE][CARD_SIZE];
    struct bitboard bingos[BINGO_TOTAL];
};

/*
 * Given an array of markers to be initialized, and a span
 * of marker values to initialize with, initialize the array
 * with a random permutation of the spanned markers.
 */
void random_markers(uint8_t *markers, int start, int end) {
    for (int m = start; m < end; m++) {
        uint32_t i = m - start;
        uint32_t j = randrange(i + 1);
        markers[i] = markers[j];
        markers[j] = m;
    }
}

struct card make_card() {
    struct card card;

    /* Select squares for this card. */
    uint8_t markers[MARKER_ROWS];
    for (int col = 0; col < CARD_SIZE; col++) {
        int base = col * MARKER_ROWS;
        random_markers(markers, base, base + MARKER_ROWS);
        for (int row = 0; row < CARD_SIZE; row++)
            card.squares[row][col] = markers[row];
    }

    /* Populate the bitboards for the winning positions. */
    struct bitboard *b;
    /* Row wins. */
    for (int row = 0; row < CARD_SIZE; row++) {
        b = &card.bingos[row];
        for (int col = 0; col < CARD_SIZE; col++)
            bitboard_setbit(b, card.squares[row][col]);
    }
    /* Column wins. */
    for (int col = 0; col < CARD_SIZE; col++) {
        b = &card.bingos[BINGO_ROWS + col];
        for (int row = 0; row < CARD_SIZE; row++)
            bitboard_setbit(b, card.squares[col][row]);
    }
    /* Negative diagonal win. */
    b = &card.bingos[BINGO_COLS];
    for (int diag = 0; diag < CARD_SIZE; diag++)
        bitboard_setbit(b, card.squares[diag][diag]);
    /* Positive diagonal win. */
    b = &card.bingos[BINGO_COLS + 1];
    for (int diag = 0; diag < CARD_SIZE; diag++)
        bitboard_setbit(b, card.squares[CARD_SIZE - diag - 1][diag]);

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

enum win_class run_game(void) {
    struct bitboard markers = bitboard_new();
    struct card card = make_card();
    uint8_t draw[NMARKERS];
    random_markers(draw, 0, NMARKERS);

    print_card(&card);
    printf("\n");

    for (int turn = 0; turn < NMARKERS; turn++) {
        uint8_t m = draw[turn];
        printf("%s\n", marker_string(m));
        bitboard_setbit(&markers, m);
        for (int b = 0; b < BINGO_TOTAL; b++) {
            if (bitboard_subset(&card.bingos[b], &markers)) {
                if (b < BINGO_ROWS)
                    return WIN_ROW;
                if (b < BINGO_COLS)
                    return WIN_COL;
                return WIN_DIAG;
            }
        }
    }

    /* All the markers are gone, and no one has bingoed. */
    assert(0);
}

int main() {
    if (!has_rdrand()) {
        fprintf(stderr, "program requires RDRAND CPU instruction: exiting\n");
        return 1;
    }
    enum win_class win = run_game();
    switch (win) {
    case WIN_ROW: printf("row win\n"); break;
    case WIN_COL: printf("col win\n"); break;
    case WIN_DIAG: printf("diag win\n"); break;
    default: assert(0);
    }
    return 0;
}
