#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define BIGINT_H
#ifdef BIGINT_H

#define BIGINT_SIZE 1024
#define BIGINT_BASE 16

#define PRINT_BI(bi)\
    printf("Number temp_res : ");\
    for (int m = 0; m < bi->num_digits; m++){\
        printf("%d ", *(bi->digits + bi->num_digits - m - 1));\
    }\
    printf("\n");\

typedef struct {
    unsigned int * digits;
    size_t num_digits;
} BigInt;

typedef enum {
    BI_EQUAL,
    BI_LESS,
    BI_GREATER
} BI_COMPARISON;

BigInt* BI_construct(unsigned long long value);
BigInt* BI_fromPoweredNumber(unsigned int numA, unsigned long long n);
BigInt* BI_fromString(const char * str);

void BI_setValueFromString(BigInt* numA , const char * str);

BigInt* BI_copy(const BigInt* numA);
void BI_copyIP(BigInt* numA, const BigInt* numB);

BI_COMPARISON BI_compare(const BigInt* numA, const BigInt* numB);

void BI_addBigIntIP(BigInt * numA, const BigInt * numB);
void BI_addNumberIP(BigInt * numA, const unsigned long long n);

int BI_subBigIntIP(BigInt * numA, const BigInt * numB);

void BI_modBigIntIP(BigInt * numA, const BigInt* numB);
void BI_modBigInt(BigInt * numR, const BigInt * numA, const BigInt* numB);

void BI_multiplyByNumber(BigInt * numR, const BigInt * numA, const unsigned int B);
void BI_multiplyByNumberIP(BigInt * numA, const unsigned int B);

void BI_multiplyByBI(BigInt * numR, const BigInt * numA, const BigInt * numB);

void BI_power(BigInt * numR, BigInt const * const numA, unsigned long long n);

void BI_shiftDigitsLeftIP(BigInt* numA, size_t s);

void BI_print(const BigInt * numA);

void BI_free(BigInt * numA);

#endif