#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include "util.h"

typedef float float32_t;
typedef double float64_t;

int main(void) {
    unsigned char buf[1024];
    int8_t magic;
    int16_t monkeycount;
    int32_t altitude;
    float32_t absurdityfactor;
    char s[] = "Great unmitigated Zot! You've found the Runestaff!";
    char s2[96];
    int16_t packetsize, ps2;

    packetsize = pack(
        buf, "chhlsf",
        (int8_t) 'B',
        (int16_t) 0,
        (int16_t) 37,
        (int32_t) -5,
        s,
        (float32_t) -3490.6677
    );

    packi16(buf + 1, packetsize);

    printf("packet is %" PRId32 "bytes\n", packetsize);
    unpack(
        buf,
        "chhl96sf",
        &magic,
        &ps2,
        &monkeycount,
        &altitude,
        s2,
        &absurdityfactor
    );

    printf(
        "'%c' %" PRId32 " %" PRId16 " %" PRId32 " \"%s\" %f\n",
        magic,
        ps2,
        monkeycount,
        altitude,
        s2,
        absurdityfactor
    );

    return EXIT_SUCCESS;
}

