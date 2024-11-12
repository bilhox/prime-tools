#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#ifndef BIGINT_H
#define BIGINT_H

typedef struct {
    unsigned int * digits;
    size_t num_digits;
} BigInt;

#define BIGINT_SIZE 1024 // Would be interesting to have dynamic allocation
#define BIGINT_BASE 16

#define MAX(a, b) ((a) < (b)) ? b : a
#define MIN(a, b) ((a) < (b)) ? a : b
#define MOD(a, b) ((a) + (b)) % b
#define LOG(a, b) log(a)/log(b)

static BigInt * num_buffer = NULL;
static BigInt * num_buffer2 = NULL; // For powering a number, because called functions in these functions already use num_buffer
static int is_initialized = 0;

#define BI_INIT_CHECK(returned)\
    if (!is_initialized)\
        return returned;\

#define RETURN_CODE_CHECK(call, returned)\
    if (call == -1)\
        return returned;\

typedef enum {
    BI_EQUAL,
    BI_LESS,
    BI_GREATER,
    BI_UNKNOWN
} BI_COMPARISON;

int BI_init();
int BI_quit();

BigInt* BI_construct(unsigned long long value);
BigInt* BI_fromPoweredNumber(unsigned int numA, unsigned long long n);

#if BIGINT_BASE == 10
BigInt* BI_fromString(const char * str); // Technically the algorithm is ready, check BI_setValueFromString
#endif

int BI_setValueFromString(BigInt* numA , const char * str);

int BI_copy(BigInt* numA, const BigInt* numB);

BI_COMPARISON BI_compare(const BigInt* numA, const BigInt* numB);

int BI_addBigIntIP(BigInt * numA, const BigInt * numB);
int BI_addNumberIP(BigInt * numA, const unsigned long long n);

int BI_subBigIntIP(BigInt * numA, const BigInt * numB);

int BI_modBigIntIP(BigInt * numA, const BigInt* numB);
int BI_modBigInt(BigInt * numR, const BigInt * numA, const BigInt* numB);

int BI_multiplyByNumber(BigInt * numR, const BigInt * numA, const unsigned int B);
int BI_multiplyByNumberIP(BigInt * numA, const unsigned int B);

int BI_multiplyByBI(BigInt * numR, const BigInt * numA, const BigInt * numB);

int BI_power(BigInt * numR, BigInt const * const numA, unsigned long long n);

int BI_shiftDigitsLeftIP(BigInt* numA, size_t s);

#if BIGINT_BASE == 10
void BI_print(const BigInt * numA);
#else
// Temporary here to print a number in a base different than 10
#define PRINT_BI(bi)\
    printf("Number temp_res : ");\
    for (int m = 0; m < bi->num_digits; m++){\
        printf("%d ", *(bi->digits + bi->num_digits - m - 1));\
    }\
    printf("\n");\

#endif

void BI_free(BigInt * numA);

#endif