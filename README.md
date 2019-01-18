# Bingomatic
Copyright (c) 2019 Bart Massey

This is a work-in-progress Bingo Hall simulator. It plays
many simultaneous Bingo games and records "bingos" as row,
column or diagonal wins.

This project was inspired by a Reddit
[thread](<https://www.reddit.com/r/rust/comments/agelwg/is_vecoptioni32_cachefriendly/ee6w7di/),
and written partly as a demo for my Systems Programming
[course](http://wiki.cs.pdx.edu/cs201-winter2019).

## Build and Run

This program currently requires the `RDRAND`
hardware-random-number CPU instruction, which is only
available on modern Intel and AMD x86-64 processors. Future
work hopefully will remove this restriction without impact
on RNG quality and with not too much impact on performance.

You should be able to type `make` and get the `bingo`
executable. `make testrdrand` yields an executable that will
do some basic sanity checking of the RNG and measure its
performance a bit.

## License

This program is licensed under the "MIT License".  Please
see the file `LICENSE` in the source distribution of this
software for license terms.
