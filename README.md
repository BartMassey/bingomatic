# Bingomatic
Copyright (c) 2019 Bart Massey

This is a work-in-progress Bingo Hall simulator. It plays
many simultaneous Bingo games and records "bingos" as row,
column or diagonal wins.

This project was inspired by a Reddit
[thread](https://www.reddit.com/r/rust/comments/agelwg/is_vecoptioni32_cachefriendly/ee6w7di/),
and written partly as a demo for my Systems Programming
[course](http://wiki.cs.pdx.edu/cs201-winter2019).

## Build and Run

This program requires successful installation of the
[`toyrand`](http://github.com/BartMassey/toyrand) PRNG.

You should be able to type `make` and get the `bingo`
executable.

## License

This program is licensed under the "MIT License".  Please
see the file `LICENSE` in the source distribution of this
software for license terms.

The file `popcnt.h` is by Kim Walisch and Wojciech Muła and
is licensed under the "Simplified (2-clause) BSD` license.
Please see this file in the source distribution for license
terms.
