#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "bigint.h"

#define MAX(a, b) (a < b) ? b : a
#define MOD(a, b) (a + b) % b
#define LOG(a, b) log(a)/log(b)

static int
_hextoint(char val)
{
    /* 'hex' is a two digit hexadecimal number, no spaces, no signs.
     * This algorithm is brute force, but it is character system agnostic.
     * It is definitely not a general purpose solution.
     */
    char new_val = 0;

    switch (toupper(val)) {
        case '0':
            break;
        case '1':
            new_val += 0x1;
            break;
        case '2':
            new_val += 0x2;
            break;
        case '3':
            new_val += 0x3;
            break;
        case '4':
            new_val += 0x4;
            break;
        case '5':
            new_val += 0x5;
            break;
        case '6':
            new_val += 0x6;
            break;
        case '7':
            new_val += 0x7;
            break;
        case '8':
            new_val += 0x8;
            break;
        case '9':
            new_val += 0x9;
            break;
        case 'A':
            new_val += 0xA;
            break;
        case 'B':
            new_val += 0xB;
            break;
        case 'C':
            new_val += 0xC;
            break;
        case 'D':
            new_val += 0xD;
            break;
        case 'E':
            new_val += 0xE;
            break;
        case 'F':
            new_val += 0xF;
            break;
    }

    return new_val;
}

void BI_setDigitsTo0(BigInt* numA, size_t n){
    if (n < BIGINT_SIZE){
        memset(numA->digits, 0, n * sizeof(unsigned int));
    }
}

BigInt* BI_construct(unsigned long long value) {


    if (value < 0)
        return NULL;

    BigInt* new_big_int = malloc(sizeof(BigInt));

    if(!new_big_int) {
        printf("failed to allocate number\n");
        return NULL;
    }

    if (value){
#if BIGINT_BASE == 10
        new_big_int->num_digits = (size_t) log10(value) + 1;
#else
        new_big_int->num_digits = (size_t) LOG(value, BIGINT_BASE) + 1;
#endif
    } else {
        new_big_int->num_digits = 1;
    }

    new_big_int->digits = (unsigned int *) malloc(sizeof(unsigned int) * BIGINT_SIZE);
    if(!new_big_int->digits) {
        printf("failed to allocate number digits\n");
        free(new_big_int);
        return NULL;
    }

    unsigned int count = new_big_int->num_digits;
    unsigned int * digits = new_big_int->digits;
    while(count--) {
        (*digits++) = value % BIGINT_BASE;
        value /= BIGINT_BASE;
    }

    // for (int m = 0; m < new_big_int->num_digits; m++){
    //     printf("%d\n", *(new_big_int->digits + new_big_int->num_digits - m - 1));
    // }

    return new_big_int;
}

void BI_setValueFromString(BigInt* numA , const char * str){

    // TODO: Find a way to parse any string to any base

    if (BIGINT_BASE == 10) {
        numA->num_digits = strlen(str);

        const char * it = str + numA->num_digits - 1;
        unsigned int * digits = numA->digits;
        while (it >= str){
            unsigned int digit = *it;
            if (digit < '0' || digit > '9'){
                BI_free(numA);
            }
            it--;

            *digits = digit - '0';
            digits++;
        }
    } else if (BIGINT_BASE != 0 && (BIGINT_BASE & (BIGINT_BASE - 1)) == 0){

        int power = (int) (LOG(BIGINT_BASE, 16));
        numA->num_digits = strlen(str) / power + 1; // Maybe here strlen can return 0, needs checks
        int residu = strlen(str) % power;
        // printf("%d %d %zu\n", power, residu, strlen(str));

        size_t i = 0;
        size_t numI = 0;
        unsigned int * A = numA->digits;

        *A = 0;

        for (int k = 0; k < residu; k++){
            *A += (unsigned int) _hextoint(*(str + strlen(str) - 1 - k)) * pow(16, k);
        }

        if (residu != 0)
            i += residu;

        while (i < strlen(str)){

            *(A + numI) = 0;

            for (int j = 0; j < power; j++){
                const char * digit = str + strlen(str) - i - j - 1;
                *(A + numI) += (unsigned int) (_hextoint(*digit) * pow(16, j));
            }

            i += power;
            numI++;
        }
    }

}

BigInt* BI_copy(const BigInt* numA){
    
    BigInt * copy = BI_construct(0);

    memcpy(copy->digits, numA->digits, numA->num_digits * sizeof(unsigned int));

    copy->num_digits = numA->num_digits;

    return copy;
}

void BI_copyIP(BigInt* numA, const BigInt* numB){

    memcpy(numA->digits, numB->digits, numB->num_digits * sizeof(unsigned int));

    numA->num_digits = numB->num_digits;
}

#if BIGINT_BASE == 10
BigInt* BI_fromString(const char* str){

    // TODO: Find a way to parse any string to any base
    
    BigInt * bigInt = malloc(sizeof(BigInt));

    if (!bigInt)
        return NULL;

    bigInt->num_digits = strlen(str);

    bigInt->digits = (unsigned int *) malloc(sizeof(unsigned int) * BIGINT_SIZE);
    if(!bigInt->digits) {
        free(bigInt);
        return NULL;
    }

    const char * it = str + bigInt->num_digits - 1;
    unsigned int * digits = bigInt->digits;
    while (it >= str){
        char digit = *it;
        if (digit < '0' || digit > '9'){
            BI_free(bigInt);
            return NULL;
        }
        it--;

        *digits = (unsigned int) digit - '0';
        digits++;
    }

    return bigInt;
}
#endif

void BI_addBigIntIP(BigInt * numA, const BigInt * numB){

    unsigned int * A = numA->digits;
    const unsigned int * B = numB->digits;

    int ret = 0;
    size_t imax = MAX(numA->num_digits, numB->num_digits);

    for (size_t i = 0; i < imax; i++){
        unsigned int * a = A + i;
        const unsigned int b = B[i];
        unsigned int res = 0;

        if (i >= numA->num_digits){
            res = (b + ret) % BIGINT_BASE;
            ret = (b + ret) / BIGINT_BASE;
        } else if (i >= numB->num_digits){
            res = (*a + ret) % BIGINT_BASE;
            ret = (*a + ret) / BIGINT_BASE;
        } else {
            res = (*a + b + ret) % BIGINT_BASE;
            ret = (*a + b + ret) / BIGINT_BASE;
        }

        *a = res;
    }

    numA->num_digits = imax;

    if (ret != 0){
        A[numA->num_digits] = (unsigned int) ret;
        numA->num_digits += 1;
    }
}

int BI_subBigIntIP(BigInt * numA, const BigInt * numB){

    unsigned int * A = numA->digits;
    const unsigned int * B = numB->digits;

    if (BI_compare(numA, numB) == BI_LESS)
        return -1;

    int ret = 0;
    size_t imax = numA->num_digits;

    for (size_t i = 0; i < imax; i++){
        unsigned int * a = A + i;
        unsigned int res = 0;

        if (i >= numB->num_digits){
            res = MOD(*a - ret, BIGINT_BASE);
            ret = (*a - ret) < 0;
        } else {
            const unsigned int b = B[i];
            res = MOD(*a - b - ret, BIGINT_BASE);
            ret = (*a - b - ret) < 0;
        }
        *a = res;
    }

    unsigned int * it = A + numA->num_digits - 1;
    while(*it == 0 && numA->num_digits != 1){
        numA->num_digits--;
        it--;
    }

    return 0;
}

void BI_modBigIntIP(BigInt * numA, const BigInt* numB){

    BigInt * temp_res = BI_construct(0);
    const unsigned int * B = numB->digits;
    unsigned int * A = numA->digits;
    long long skipped_digits = 0;

    while(BI_compare(numA, numB) != BI_LESS){

        if (numB->num_digits < numA->num_digits){
            skipped_digits = 0;
            unsigned int last_digit_A = A[numA->num_digits - 1]; 
            unsigned int last_digit_B = B[numB->num_digits - 1]; 
            BI_copyIP(temp_res, numB);

            if (last_digit_A > last_digit_B)
                skipped_digits = numA->num_digits - numB->num_digits;
            else
                skipped_digits = numA->num_digits - numB->num_digits - 1;
            
            skipped_digits = MAX(0, skipped_digits);
            BI_shiftDigitsLeftIP(temp_res, (size_t) skipped_digits);
            BI_subBigIntIP(numA, temp_res);

        } else {
            BI_subBigIntIP(numA, numB);
        }
    }

    BI_free(temp_res);
}

void BI_modBigInt(BigInt * numR, const BigInt * numA, const BigInt* numB){

    BI_copyIP(numR, numA);

    BigInt * temp_res = BI_construct(0);
    const unsigned int * B = numB->digits;
    unsigned int * R = numR->digits;
    long long skipped_digits = 0;

    while(BI_compare(numA, numB) != BI_LESS){

        if (numB->num_digits < numA->num_digits){
            skipped_digits = 0;
            unsigned int last_digit_R = R[numR->num_digits - 1]; 
            unsigned int last_digit_B = B[numB->num_digits - 1]; 
            BI_copyIP(temp_res, numB);

            if (last_digit_R > last_digit_B)
                skipped_digits = numR->num_digits - numB->num_digits;
            else
                skipped_digits = numR->num_digits - numB->num_digits - 1;
            
            skipped_digits = MAX(0, skipped_digits);
            BI_shiftDigitsLeftIP(temp_res, (size_t) skipped_digits);
            BI_subBigIntIP(numR, temp_res);

        } else {
            BI_subBigIntIP(numR, numB);
        }
    }

    BI_free(temp_res);
}

void BI_addNumberIP(BigInt * numA, const unsigned long long n){

    BigInt * numB = BI_construct(n);

    unsigned int * A = numA->digits;
    const unsigned int * B = numB->digits;

    int ret = 0;
    size_t imax = MAX(numA->num_digits, numB->num_digits);

    for (size_t i = 0; i < imax; i++){
        unsigned int * a = A + i;
        const unsigned int b = B[i];

        unsigned int res = (*a + b + ret) % BIGINT_BASE;
        ret = (*a + b + ret) / BIGINT_BASE;

        *a = res;
    }

    numA->num_digits = imax;

    if (ret != 0){
        A[numA->num_digits] = (unsigned int) ret;
        numA->num_digits += 1;
    }

    BI_free(numB);
}

void BI_shiftDigitsLeftIP(BigInt* numA, size_t s){
    
    if (s == 0)
        return;

    if (s >= BIGINT_SIZE){
        memset(numA->digits, 0, sizeof(unsigned int) * BIGINT_SIZE);
        return;
    }

    for (size_t i = numA->num_digits + s - 1; i >= s; i--){
        *(numA->digits + i) = *(numA->digits + i - s);
    }

    for (size_t j = 0; j < s; j++){
        numA->digits[j] = 0;
    }

    numA->num_digits += s;
}

void BI_multiplyByBI(BigInt * numR, const BigInt * numA, const BigInt * numB){

    const unsigned int * A = numA->digits;
    const unsigned int * B = numB->digits;

    BigInt * intermediate_result = BI_construct(0);

    BI_setValueFromString(numR, "0");

    for (size_t i = 0; i < numB->num_digits; i++){
        const unsigned int multiplier = numB->digits[i];

        BI_multiplyByNumber(intermediate_result, numA, multiplier);

        BI_shiftDigitsLeftIP(intermediate_result, i);

        BI_addBigIntIP(numR, intermediate_result);
        // BI_print(intermediate_result);
    }

    BI_free(intermediate_result);

}

BigInt* BI_fromPoweredNumber(unsigned int numA, unsigned long long n){

    if (numA > 9 || numA == 0)
        return NULL;

    BigInt* result = BI_construct(1);

    if (n == 0)
        return result;

    while(n--){
        BI_multiplyByNumberIP(result, numA);
    }

    return result;
}

void BI_power(BigInt* numR, BigInt const * const numA, unsigned long long n){

    BI_setValueFromString(numR, "1");
    // printf("%zu\n", numR->num_digits);

    BigInt* intermediate_result = BI_construct(1);

    while(n--){
        BI_multiplyByBI(intermediate_result, numR, numA);
        BI_copyIP(numR, intermediate_result);
    }
}

// void BI_powerIP(const BigInt * numA, unsigned long long n){
    
//     BigInt* result = BI_construct(1);

//     while(n--){
//         BigInt* res = BI_multiplyByBI(result, numA);
//         BI_copyIP(result, res);
//     }

//     return result;
// }

void BI_multiplyByNumberIP(BigInt * numA, const unsigned int B){

    unsigned int * A = numA->digits;

    if (B == 0){
        for (size_t i = 0; i < numA->num_digits; i++){
            A[i] = 0;
        }

        numA->num_digits = 0;
        return;
    }

    int ret = 0;
    for (size_t i = 0; i < numA->num_digits; i++){
        unsigned int * a = A + i;

        unsigned int res = (*a * B + ret) % BIGINT_BASE;
        ret = (*a * B + ret) / BIGINT_BASE;

        *a = res;
    }

    while (ret != 0){
        A[numA->num_digits] = (unsigned int) ret;
        numA->num_digits += 1;
        ret /= BIGINT_BASE;
    }
}

// Au lieu de recréer une instance à chaque function call, prendre en argument un BigInt de destination

void BI_multiplyByNumber(BigInt * numR, const BigInt * numA, const unsigned int B){

    const unsigned int * A = numA->digits;

    if (B == 0){
        BI_setValueFromString(numR, "0");
        return;
    }

    numR->num_digits = numA->num_digits;

    int ret = 0;
    for (size_t i = 0; i < numA->num_digits; i++){
        const unsigned int * a = A + i;
        unsigned int * r = numR->digits + i;

        unsigned int res = (*a * B + ret) % BIGINT_BASE;
        ret = (*a * B + ret) / BIGINT_BASE;

        *r = res;
    }

    while (ret != 0){
        numR->digits[numA->num_digits] = (unsigned int) ret;
        numR->num_digits += 1;
        ret /= BIGINT_BASE;
    }
}

void BI_print(const BigInt * numA){

    unsigned int * digits = numA->digits;

    for(size_t i = 0; i < numA->num_digits; i++){
        fputc('0' + digits[numA->num_digits - i - 1], stdout);
    }
    fputc('\n', stdout);
}

void BI_free(BigInt * numA){
    free(numA->digits);
    free(numA);
}

BI_COMPARISON BI_compare(const BigInt* numA, const BigInt* numB){

    const unsigned int * A = numA->digits;
    const unsigned int * B = numB->digits;

    if (numA->num_digits < numB->num_digits){
        return BI_LESS;
    } else if (numA->num_digits > numB->num_digits){
        return BI_GREATER;
    }

    // From here we know they both have the same number of digits

    size_t it = numA->num_digits;
    unsigned int a = 0;
    unsigned int b = 0;
    while (it--){
        a = *(A + it);
        b = *(B + it);

        if (a < b)
            return BI_LESS;
        else if (a > b)
            return BI_GREATER;
    }

    return BI_EQUAL;
}