/*
 * Fast Monte-Carlo Bingo Sim inspired by
 * https://www.reddit.com/r/rust/
 *    comments/agelwg/is_vecoptioni32_cachefriendly/ee6w7di/
 *
 * Written in C for my CS 201 course Winter 2019. Rust would
 * be a better choice otherwise.
 */

#include <stdio.h>

int main() {
    __uint128_t v = 1LL << 127;
    printf("%llx\n", v);
    return 0;
}
