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

#include "bingo.h"

#ifdef LOGGING
#define LOG(...) (printf(__VA_ARGS__))
#else
#define LOG(...) /**/
#endif

#define MARKER_ROWS 15
#define NMARKERS (CARD_SIZE * MARKER_ROWS)

enum bingos {
    BINGO_ROWS = CARD_SIZE,
    BINGO_COLS = BINGO_ROWS + CARD_SIZE,
    BINGO_TOTAL = BINGO_COLS + 2,
};

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

    /* Initialize the markings. */
    card.markings = malloc(sizeof *card.markings);
    assert(card.markings);

    /* Set up the bitboard. */
    bitboard_clear(card.markings);
    for (int row = 0; row < CARD_SIZE; row++)
        for (int col = 0; col < CARD_SIZE; col++)
            bitboard_set(card.markings, card.squares[row][col]);

    /* Set up the indices. (This is the tricky part.) */
    for (int row = 0; row < CARD_SIZE; row++) {
        for (int col = 0; col < CARD_SIZE; col++) {
            uint8_t incr = (row << 3) | col;
            if (row == col)
                incr |= 0x80;
            if (CARD_SIZE - 1 - row == col)
                incr |= 0x40;
            int index = bitboard_mark(card.markings, card.squares[row][col]);
            assert(index != -1);
            card.markings->increments[index] = incr;
        }
    }

    /* Clear counters for initial use. (Must be reset if the
       board is reused.) */
    card.markings->d_counters = 0;
    card.markings->rc_counters = 0;

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

static void print_card(struct card *card) {
    for (int row = 0; row < CARD_SIZE; row++) {
        char *sep = "";
        for (int col = 0; col < CARD_SIZE; col++) {
            printf("%s%s", sep, marker_string(card->squares[row][col]));
            sep = " ";
        }
        printf("\n");
    }
    printf("\n");
}
#endif

/* Number of turns played so far. */
uint64_t turns_counter;

/* Number of wins by row, column, or diagonal. */
uint64_t win_counters[3][5];

enum win_indices {
    WIN_ROW,
    WIN_COL,
    WIN_DIAG,
};

enum diagonals {
    P_DIAG,
    N_DIAG,
};

static void run_game(struct card *card, struct toyrand_pool *pool) {
    assert(CARD_SIZE == 5);

    /* Set up the card. */
    if (!card) {
        struct card new_card = make_card(pool);
        card = &new_card;
    }
    card->markings->d_counters = 0;
    card->markings->rc_counters = 0;

    /* Draw all the markers. */
    uint8_t draw[NMARKERS];
    random_markers(pool, draw, 0, NMARKERS);

#ifdef LOGGING
    print_card(card);
#endif

    /* Run the game. */
    for (int turn = 0; turn < NMARKERS; turn++) {
        uint8_t m = draw[turn];
        LOG("%s\n", marker_string(m));

        /* Check for hit. */
        int index = bitboard_mark(card->markings, m);
        if (index == -1)
            continue;

        /* Set up for counter test / increment. */
        uint8_t incr = card->markings->increments[index];
        int row = (incr >> 3) & 0x7;
        int col = incr & 0x7;
        int n_diag = (incr >> 7) & 1;
        int p_diag = (incr >> 6) & 1;
        uint64_t counter_add = 1L << (3 * col);
        counter_add |= 1L << (3 * (CARD_SIZE + row));
        counter_add |= (uint64_t) (p_diag | (n_diag << 3)) << (6 * CARD_SIZE);

        /* Test for win. */
        uint64_t counters = card->markings->rc_counters;
        counters |= (uint64_t) (card->markings->d_counters) << (6 * CARD_SIZE);
        /* Check for some counter to be incremented but
           already equal to 4. */
        if (counters & (counter_add << 2)) {
            /*
             * Note that there could be "ties" here: row,
             * column and 1 or 2 diagonal wins at the same time.
             * We will count them all.
             */
            if (counters & (1L << (3 * col + 2)))
                win_counters[WIN_COL][col]++;
            if (counters & (1L << (3 * (CARD_SIZE + row) + 2)))
                win_counters[WIN_ROW][row]++;
            if (counters & (1L << (6 * CARD_SIZE + 2)))
                win_counters[WIN_DIAG][P_DIAG]++;
            if (counters & (1L << (6 * CARD_SIZE + 5)))
                win_counters[WIN_DIAG][N_DIAG]++;
            turns_counter += turn;
            return;
        }

        /* Bump the counts and put them back. */
        counters += counter_add;
        card->markings->rc_counters =
            counters & ((1L << (6 * CARD_SIZE + 1)) - 1);
        card->markings->d_counters = counters >> (6 * CARD_SIZE);
    }

    /* All the markers are gone, yet no bingo. */
    assert(0);
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

#ifndef RECARD
    struct card new_card = make_card(pool);
#endif
    for (int i = 0; i < ngames; i++) {
        LOG("game %d:\n", i);
#ifndef RECARD
        run_game(&new_card, pool);
#else
        run_game(0, pool);
#endif
    }

    uint64_t row_total = 0;
    for (int i = 0; i < CARD_SIZE; i++) {
        uint64_t c = win_counters[WIN_ROW][i];
        printf("row %d: %ld\n", i, c);
        row_total += c;
    }
    printf("row total: %ld\n", row_total);
    uint64_t col_total = 0;
    for (int i = 0; i < CARD_SIZE; i++) {
        uint64_t c = win_counters[WIN_COL][i];
        printf("col %d: %ld\n", i, c);
        col_total += c;
    }
    printf("col total: %ld\n", col_total);
    uint64_t pdiag = win_counters[WIN_DIAG][P_DIAG];
    printf("pos diag: %ld\n", pdiag);
    uint64_t ndiag = win_counters[WIN_DIAG][N_DIAG];
    printf("neg diag: %ld\n", ndiag);
    printf("total: %ld\n", row_total + col_total + pdiag + ndiag);
    printf("avg turns: %g\n", (double) turns_counter / ngames);

    return 0;
}
