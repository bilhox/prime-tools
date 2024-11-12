#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include "bigint.h"

int BI_init(){

    is_initialized = 1;

    num_buffer = BI_construct(0);
    num_buffer2 = BI_construct(0);

    if (num_buffer == NULL || num_buffer2 == NULL)
        return -1;

    return 0;
}

int BI_quit(){

    is_initialized = 0;

    BI_free(num_buffer);
    BI_free(num_buffer2);

    return 0;
}

static int
_hextoint(char val)
{
    /* 'hex' is a one digit hexadecimal number, no spaces, no signs.
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

    size_t count = new_big_int->num_digits;
    unsigned int * digits = new_big_int->digits;
    while(count--) {
        (*digits++) = value % BIGINT_BASE;
        value /= BIGINT_BASE;
    }

    return new_big_int;
}

int BI_setValueFromString(BigInt* numA, const char * str){

    if (str == NULL || numA == NULL)
        return -1;

    if (strlen(str) > BIGINT_SIZE)
        return -1; // Maybe -2 so the user can understand the string too big

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
        numA->num_digits = strlen(str) / power; // Maybe here strlen can return 0, needs checks
        int residu = strlen(str) % power;

        if (residu != 0){
            numA->num_digits += 1;
        }

        long long i = (long long) strlen(str) - 1; // str iterator
        unsigned int * it = numA->digits; // bigint interator

        while (i + 1 > residu){

            *(it) = 0;

            unsigned int s = 0;

            for (int j = 0; j < power; j++){
                const char * digit = str + i - j;
                s += (unsigned int) (_hextoint(*digit) * pow(16, j));
            }

            *it = s;

            i -= power;
            it++;
        }

        if (residu != 0) {
            *it = 0;

            for (int k = 0; k < residu; k++){
                *it += (unsigned int) _hextoint(*(str + i - k)) * pow(16, k);
            }
        }
    }

    return 0;
}

int BI_copy(BigInt* numA, const BigInt* numB){

    if (numA == NULL || numB == NULL)
        return -1;

    memcpy(numA->digits, numB->digits, numB->num_digits * sizeof(unsigned int));

    numA->num_digits = numB->num_digits;

    return 0;
}

#if BIGINT_BASE == 10
BigInt* BI_fromString(const char* str){

    // Does not need init check as it doesn't use any buffers
    // TODO: Find a way to parse any string to any base

    if (str == NULL)
        return NULL;

    if (strlen(str) > BIGINT_SIZE)
        return NULL;

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

int BI_addBigIntIP(BigInt * numA, const BigInt * numB){

    if (numA == NULL || numB == NULL)
        return -1;

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
        if (numA->num_digits == BIGINT_SIZE - 1)
            return -1; // I leave the result unfinished, it's to the user to handle it with the returned code

        A[numA->num_digits] = (unsigned int) ret;
        numA->num_digits += 1;
    }

    return 0;
}

int BI_subBigIntIP(BigInt * numA, const BigInt * numB){

    if (numA == NULL || numB == NULL)
        return -1;

    unsigned int * A = numA->digits;
    const unsigned int * B = numB->digits;

    // PRINT_BI(numA);

    if (BI_compare(numA, numB) == BI_LESS)
        return -2;

    int ret = 0;
    size_t imax = numB->num_digits;

    for (size_t i = 0; i < imax || ret != 0; i++){
        unsigned int * a = A + i;
        int res = 0;

        if (i >= numB->num_digits){
            res = MOD(*a - ret, BIGINT_BASE);
            ret = (int) (*a - ret) < 0;
        } else {
            const unsigned int b = B[i];
            res = MOD(*a - b - ret, BIGINT_BASE);
            ret = (int) ( *a - b - ret) < 0;
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

int BI_modBigIntIP(BigInt * numA, const BigInt* numB){

    if (numA == NULL || numB == NULL)
        return -1;

    BI_INIT_CHECK(-1)

    const unsigned int * B = numB->digits;
    unsigned int * A = numA->digits;
    long long skipped_digits = 0;

    int sub_returned_code = 0;

    while(BI_compare(numA, numB) != BI_LESS){

        if (numB->num_digits < numA->num_digits){
            skipped_digits = 0;
            unsigned int last_digit_A = A[numA->num_digits - 1];
            unsigned int last_digit_B = B[numB->num_digits - 1];

            RETURN_CODE_CHECK(BI_copy(num_buffer, numB), -1)

            if (last_digit_A > last_digit_B)
                skipped_digits = numA->num_digits - numB->num_digits;
            else
                skipped_digits = numA->num_digits - numB->num_digits - 1;

            skipped_digits = MAX(0, skipped_digits);

            RETURN_CODE_CHECK(BI_shiftDigitsLeftIP(num_buffer, (size_t) skipped_digits), -1)

            sub_returned_code = BI_subBigIntIP(numA, num_buffer);

        } else {
            sub_returned_code = BI_subBigIntIP(numA, numB);
        }

        if (sub_returned_code == -1)
            return -1;
    }

    return 0;
}

int BI_modBigInt(BigInt * numR, const BigInt * numA, const BigInt* numB){

    if(BI_copy(numR, numA) == -1)
        return -1;

    BI_INIT_CHECK(-1)

    const unsigned int * B = numB->digits;
    unsigned int * R = numR->digits;
    long long skipped_digits = 0;

    int sub_returned_code = 0;

    while(BI_compare(numR, numB) != BI_LESS){

        if (numB->num_digits < numR->num_digits){
            skipped_digits = 0;
            unsigned int last_digit_R = R[numR->num_digits - 1];
            unsigned int last_digit_B = B[numB->num_digits - 1];

            RETURN_CODE_CHECK(BI_copy(num_buffer, numB), -1)

            if (last_digit_R > last_digit_B)
                skipped_digits = numR->num_digits - numB->num_digits;
            else
                skipped_digits = numR->num_digits - numB->num_digits - 1;

            skipped_digits = MAX(0, skipped_digits);

            RETURN_CODE_CHECK(BI_shiftDigitsLeftIP(num_buffer, (size_t) skipped_digits), -1)

            sub_returned_code = BI_subBigIntIP(numR, num_buffer);

        } else {
            sub_returned_code = BI_subBigIntIP(numR, numB);
        }

        if (sub_returned_code == -1)
            return -1;
    }

    return 0;

}

int BI_addNumberIP(BigInt * numA, const unsigned long long n){

    // needs optimization, no need for new BI

    BigInt * numB = BI_construct(n);

    if (numB == NULL || numA == NULL)
        return -1;

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
        if (numA->num_digits == BIGINT_SIZE - 1)
            return -1; // I leave the result unfinished, it's to the user to handle it with the returned code
        A[numA->num_digits] = (unsigned int) ret;
        numA->num_digits += 1;
    }

    BI_free(numB);

    return 0;
}

int BI_shiftDigitsLeftIP(BigInt* numA, size_t s){

    if (numA == NULL)
        return -1;

    if (s == 0 || (numA->num_digits == 1 && numA->digits[0] == 0))
        return 0;

    if (s >= BIGINT_SIZE){
        memset(numA->digits, 0, sizeof(unsigned int) * BIGINT_SIZE);
        return 0;
    }

    size_t digits_to_shift = numA->num_digits;
    numA->num_digits = MIN(numA->num_digits + s, BIGINT_SIZE);

    if (digits_to_shift + s > BIGINT_SIZE)
        digits_to_shift = BIGINT_SIZE - s;

    memmove(numA->digits + s, numA->digits, digits_to_shift * sizeof(unsigned int));
    memset(numA->digits, 0, s * sizeof(unsigned int));

    return 0;
}

int BI_multiplyByBI(BigInt * numR, const BigInt * numA, const BigInt * numB){

    BI_INIT_CHECK(-1)

    if (numA == NULL || numB == NULL || numR == NULL)
        return -1;

    const unsigned int * A = numA->digits;
    const unsigned int * B = numB->digits;

    RETURN_CODE_CHECK(BI_setValueFromString(numR, "0"), -1);

    for (size_t i = 0; i < numB->num_digits; i++){
        const unsigned int multiplier = numB->digits[i];

        if (BI_multiplyByNumber(num_buffer, numA, multiplier) == -1)
            return -1;

        RETURN_CODE_CHECK(BI_shiftDigitsLeftIP(num_buffer, i), -1)

        RETURN_CODE_CHECK(BI_addBigIntIP(numR, num_buffer), -1)
    }

    return 0;
}

BigInt* BI_fromPoweredNumber(unsigned int numA, unsigned long long n){

    BI_INIT_CHECK(NULL)

    if (numA > 9 || numA == 0)
        return NULL;

    BigInt* result = BI_construct(1);

    if (result == NULL)
        return NULL;

    if (n == 0)
        return result;

    while(n--){
        if (BI_multiplyByNumberIP(result, numA) == -1)
            return NULL;
    }

    return result;
}

int BI_power(BigInt* numR, BigInt const * const numA, unsigned long long n){

    BI_INIT_CHECK(-1);

    if (numA == NULL || numR == NULL)
        return -1;

    RETURN_CODE_CHECK(BI_setValueFromString(numR, "1"), -1)

    RETURN_CODE_CHECK(BI_setValueFromString(num_buffer2, "1"), -1)

    while(n--){
        RETURN_CODE_CHECK(BI_multiplyByBI(num_buffer2, numR, numA), -1)
        RETURN_CODE_CHECK(BI_copy(numR, num_buffer2), -1)
    }

    return 0;
}

int BI_multiplyByNumberIP(BigInt * numA, const unsigned int B){

    if (numA == NULL)
        return -1;

    unsigned int * A = numA->digits;

    if (B == 0){

        numA->digits[0] = 0;
        numA->num_digits = 1;

        return 0;
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
        if (numA->num_digits == BIGINT_SIZE)
            return -1;

        numA->num_digits += 1;
        ret /= BIGINT_BASE;
    }

    return 0;
}

int BI_multiplyByNumber(BigInt * numR, const BigInt * numA, const unsigned int B){

    if (numR == NULL || numA == NULL)
        return -1;

    const unsigned int * A = numA->digits;

    if (B == 0){
        numR->digits[0] = 0;
        numR->num_digits = 1;
        return 0;
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
        if (numR->num_digits == BIGINT_SIZE)
            return -1;

        numR->num_digits += 1;
        ret /= BIGINT_BASE;
    }

    return 0;
}

#if BIGINT_BASE == 10
void BI_print(const BigInt * numA){

    if (numA == NULL){
        printf("NULL\n");
        return;
    }

    unsigned int * digits = numA->digits;

    for(size_t i = 0; i < numA->num_digits; i++){
        fputc('0' + digits[numA->num_digits - i - 1], stdout);
    }
    fputc('\n', stdout);
}
#endif

void BI_free(BigInt * numA){
    free(numA->digits);
    free(numA);
}


BI_COMPARISON BI_compare(const BigInt* numA, const BigInt* numB){

    if (numA == NULL || numB == NULL)
        return BI_UNKNOWN;

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