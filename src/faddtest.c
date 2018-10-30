#include <stdio.h>
#include <stdlib.h>
#include "utils.h"

void addff(float a, float b, float *x)
{
    unsigned char ha[4], hb[4];
    long e1, e2, e3, f1, f2, f3, s3, full1, full2, i;

    /* Little-endian */
    if (a != 0) {
        ha[3] = ((unsigned char *)&a)[0];
        ha[2] = ((unsigned char *)&a)[1];
        ha[1] = ((unsigned char *)&a)[2];
        ha[0] = ((unsigned char *)&a)[3];
        full1 = (ha[0] << 24) | (ha[1] << 16) | (ha[2] << 8) | (ha[3]);
        e1 = ((full1 & 0x7F800000) >> 23) - 127;
        f1 = ((full1 & 0x7FFFFF)) + 8388608;  /* 2^13 */
    } else {
        e1 = 0;
        f1 = 0;
    }

    if (b != 0) {
        hb[3] = ((unsigned char *)&b)[0];
        hb[2] = ((unsigned char *)&b)[1];
        hb[1] = ((unsigned char *)&b)[2];
        hb[0] = ((unsigned char *)&b)[3];

        full2 = (hb[0] << 24) | (hb[1] << 16) | (hb[2] << 8) | (hb[3]);
        e2 = ((full2 & 0x7F800000) >> 23) - 127;
        f2 = ((full2 & 0x7FFFFF)) + 8388608;/* 2^13 */
    } else {
        e2 = 0;
        f2 = 0;
    }

    while (e1 < e2) {
        e1 = e1 + 1;
        f1 = f1 >> 1;
    }

    while (e2 < e1) {
        e2 = e2 + 1;
        f2 = f2 >> 1;
    }
    e3 = e1;

    f1 = (a < 0) ? -f1 : f1;
    f2 = (b < 0) ? -f2 : f2;
    f3 = f1 + f2;

    /* i will use 1 and -1 for + & - sign of the number instead of 0 & 1 */
    if (f3 < 0) {
        f3 = -f3;
        s3 = -1;
    } else {
        s3 = 1;
    }
  /* make a binary string for the fractional part */
    num2bin(f3);
    if (f3) {
        for (i = 0; i < 9; i++) {
            if (bin[i] == '1') {
                f3 = f3 >> 1;
                e3 = e3 + 1;
                num2bin(f3);
            }
        }
        while (bin[7] != '1') {
            f3 = f3 << 1;
            e3 = e3 - 1;
            num2bin(f3);
        }
    }
    *x = 0;
    for (i = 0; i < BIN_SIZE; i++) {
        *x = (bin[i] == '1') ? *x + 1.f/(1 << i) : *x;
    }
    *x = *x * (1 << (e3+8)) * s3;
}

void num2bin(long f3)
{
    unsigned long i, z;
    z = 0x80000000;
    for (i = 0; i < BIN_SIZE; i++) {
        bin[i] = ((f3 & z)) ? '1' : '0';
        z = z >> 1;
    }
    bin[i] = '\0';
}

void add_c(float a, float b, float *x) {
    *x = a+b;
}

int main(int argc, char *argv[])
{
    float a, b, x, y, z;
    /**
     ** WHERE: > z holds the result of built-in c 32-bit implementation.
     **        > y holds the result from the c-algorithm implementation.
     **        > x holds the result from the NASM/asm algorithm implementation.
     **/

    unsigned int i;
    FILE *f;
    if (argc == 2) {
        if ((f = fopen(argv[1], "r"))) {
            for(i = 0; i < HEIGHT; i++)  {
                fscanf(f,"%f",&a); // read the first operand from the file
                fscanf(f,"%f",&b); // read the second operand from the file

                addff(a, b, &x);   // add using the c-implementation of the algorithm
                addf(a, b, &y);    // add using the asm-implementation of the algorithm
                add_c(a, b, &z);   // add using the built in c 32-bit implementation

                printf("%.8f,%.8f,%.8f,%.8f, %.8f\n", a, b, x, y, z);
            }
            fclose(f);
        } else {
            fprintf(stderr, ".: could not open the file: <%s>", argv[0]);
        }
    } else {
        puts(".: usage: ./faddtest <imput test file>");
    }
    return EXIT_SUCCESS;
}
