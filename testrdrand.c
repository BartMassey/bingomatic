/* Check that the inline asm `rdrand` is actually OK. */

#include <assert.h>
#include <stdint.h>
#include <stdio.h>

#include "rdrand.h"

int main() {
    assert(has_rdrand());

    for (int i = 0; i < 20; i++)
        printf("%lx\n", rdrand64_checked());
    printf("\n");

    for (int i = 0; i < 20; i++)
        printf(" %d", randrange(3));
    printf("\n\n");

    uint32_t x = 0;
    for (int i = 0; i < 10000000; i++)
        x ^= randrange(~0);
    printf("%x\n", x);
    return 0;
}
