/*
 * Copyright © 2019 Bart Massey
 * [This program is licensed under the "MIT License"]
 * Please see the file LICENSE in the source
 * distribution of this software for license terms.
 */

/* Fast Monte-Carlo Bingo sim. */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <toyrand.h>

#include "bitboard.h"

#ifdef LOGGING
#define LOG(...) (printf(__VA_ARGS__))
#else
#define LOG(...) /**/
#endif

#define CARD_SIZE 5
#define MARKER_ROWS 15
#define NMARKERS (CARD_SIZE * MARKER_ROWS)

/*
 * Storage order for bingo bitboards:
 *   Row 0, 1 .. 4
 *   Col 0, 1 .. 4
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
    WIN_TOTAL,
};

struct markings {
    uint8_t increments[CARD_SIZE * CARD_SIZE];
    uint8_t d_counters;
    uint16_t markers_high;
    uint32_t rc_counters;
    uint64_t markers_low;
}

struct card {
    uint8_t squares[CARD_SIZE][CARD_SIZE];
    struct markings *markings;
};

/*
 * Given an array of markers to be initialized, and a span
 * of marker values to initialize with, initialize the array
 * with a random permutation of the spanned markers.
 */
static void random_markers(struct toyrand_pool *pool,
                           uint8_t *markers,
                           int start, int end)
{
    for (int m = start; m < end; m++) {
        uint32_t i = m - start;
        uint32_t j = toyrand_randrange32(pool, i + 1);
        markers[i] = markers[j];
        markers[j] = m;
    }
}

static struct card make_card(struct toyrand_pool *pool) {
    struct card card;

    /* Select squares for this card. */
    uint8_t markers[MARKER_ROWS];
    for (int col = 0; col < CARD_SIZE; col++) {
        int base = col * MARKER_ROWS;
        random_markers(pool, markers, base, base + MARKER_ROWS);
        for (int row = 0; row < CARD_SIZE; row++)
            card.squares[row][col] = markers[row];
    }

    /* Populate the bitboards for the winning positions. */
    struct bitboard *b;
    /* Row wins. */
    for (int row = 0; row < CARD_SIZE; row++) {
        b = &card.bingos[row];
        *b = bitboard_new();
        for (int col = 0; col < CARD_SIZE; col++)
            bitboard_setbit(b, card.squares[row][col]);
    }
    /* Column wins. */
    for (int col = 0; col < CARD_SIZE; col++) {
        b = &card.bingos[BINGO_ROWS + col];
        *b = bitboard_new();
        for (int row = 0; row < CARD_SIZE; row++)
            bitboard_setbit(b, card.squares[row][col]);
    }
    /* Negative diagonal win. */
    b = &card.bingos[BINGO_COLS];
    *b = bitboard_new();
    for (int diag = 0; diag < CARD_SIZE; diag++)
        bitboard_setbit(b, card.squares[diag][diag]);
    /* Positive diagonal win. */
    b = &card.bingos[BINGO_COLS + 1];
    *b = bitboard_new();
    for (int diag = 0; diag < CARD_SIZE; diag++)
        bitboard_setbit(b, card.squares[CARD_SIZE - diag - 1][diag]);

    return card;
}

#ifdef LOGGING
/*
 * XXX This function returns a pointer to static data whose
 * contents are overwritten on each call.
 */
static char *marker_string(uint8_t m) {
    assert(CARD_SIZE == 5);
    static char buf[5];
    char *colnames = "BINGO";
    // Numbers start at 1.
    int col = m / MARKER_ROWS;
    int w = snprintf(buf, sizeof buf, "%c%02u", colnames[col], m + 1);
    assert(w <= sizeof buf);
    return buf;
}
#endif

static void print_card(struct card *card) {
#ifndef LOGGING
    return;
#else
    for (int row = 0; row < CARD_SIZE; row++) {
        char *sep = "";
        for (int col = 0; col < CARD_SIZE; col++) {
            printf("%s%s", sep, marker_string(card->squares[row][col]));
            sep = " ";
        }
        printf("\n");
    }
    printf("\n");
#endif
}

static enum win_class run_game(struct card *card, struct toyrand_pool *pool) {
    struct bitboard markers = bitboard_new();
    if (!card) {
        struct card new_card = make_card(pool);
        card = &new_card;
    }
    uint8_t draw[NMARKERS];
    random_markers(pool, draw, 0, NMARKERS);

    print_card(card);

    for (int turn = 0; turn < NMARKERS; turn++) {
        uint8_t m = draw[turn];
        LOG("%s\n", marker_string(m));
        bitboard_setbit(&markers, m);
        for (int b = 0; b < BINGO_TOTAL; b++) {
            if (bitboard_subset(&card->bingos[b], &markers)) {
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

static void print_win(enum win_class win) {
#ifndef LOGGING
    return;
#else
    switch (win) {
    case WIN_ROW: LOG("row win\n\n"); break;
    case WIN_COL: LOG("col win\n\n"); break;
    case WIN_DIAG: LOG("diag win\n\n"); break;
    default: assert(0);
    }
#endif
}

int main(int argc, char **argv) {
    int ngames;
    if (argc != 2 || (ngames = atoi(argv[1])) <= 0) {
        fprintf(stderr, "bingo: usage: bingo <ngames>\n");
        return 1;
    }
    LOG("%d games\n\n", ngames);
    /* Needs minimum of 364 bits; we round up and double to
       get 32 * 4 * 8 = 1024 bits. */
    struct toyrand_pool *pool = toyrand_make_pool(32);
    assert(pool);
    uint64_t win_counts[WIN_TOTAL] = {0, 0, 0};

#ifndef RECARD
    struct card new_card = make_card(pool);
#endif
    for (int i = 0; i < ngames; i++) {
        LOG("game %d:\n", i);
#ifndef RECARD
        enum win_class win = run_game(&new_card, pool);
#else
        enum win_class win = run_game(0, pool);
#endif
        win_counts[win]++;
        print_win(win);
    }

    printf("%ld row\n", win_counts[WIN_ROW]);
    printf("%ld col\n", win_counts[WIN_COL]);
    printf("%ld diag\n", win_counts[WIN_DIAG]);
    return 0;
}
