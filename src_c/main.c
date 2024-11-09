#include <stdio.h>
#include <math.h>
#include <stdint.h>
#include "bigint.h"

int prime_a_b(uint64_t a, uint64_t b){

    if (a < 1 || b < 1)
        return 0;

    for (size_t i = 2; i < sqrt(b); i++){
        if (a % i == 0 && b % i == 0)
            return 0;
    }

    return 1;
}

int lucas_heimer(unsigned long long p){
    BigInt * C2 = BI_construct(2);
    BigInt * C1 = BI_construct(1);
    BigInt * C0 = BI_construct(0);

    BigInt * Mp = BI_fromPoweredNumber(2, p);
    BI_subBigIntIP(Mp, C1);

    BigInt * S = BI_construct(4);

    size_t imax = (p < 2) ? 0 : (p - 2);
    size_t i = 0;

    BigInt * temp_res = BI_construct(0);

    do {
        // printf("powering\n");
        BI_power(temp_res, S, 2);
        // printf("loop\n");
        // printf("copying\n");
        BI_copyIP(S, temp_res);
        // printf("subtracting\n");
        BI_subBigIntIP(S, C2);
        // printf("modulo\n");
        BI_modBigIntIP(S, Mp);
        // printf("end modulo\n");
        i++;
    } while (i < imax);

    // printf("comparing\n");
    int result = BI_compare(S, C0) == BI_EQUAL;

    // printf("freeing\n");
    BI_free(C0);
    BI_free(C1);
    BI_free(C2);
    BI_free(Mp);
    BI_free(S);

    return result;
}

int main(int argc, char * argv[]){

    for (size_t p = 3; p <= 200; p++){
        if(lucas_heimer(p)){
            printf("Mersenne number 2^%zu is prime\n", p);
        }
    }

    return 0;
}