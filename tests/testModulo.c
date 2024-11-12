#include "testModulo.h"

#include "unity.h"
#include "bigint.h"

void test_Nominal_SmallInts_InPlace(void)
{
    BigInt* first  = BI_construct(1e6);
    BigInt* second = BI_construct(30);

    BigInt* copy = BI_construct(0);
    BI_copy(copy, first);

    BigInt* expected = BI_construct(10U);

    BI_modBigIntIP(first, second);

    TEST_ASSERT_EQUAL(BI_compare(copy, first), BI_GREATER);
    TEST_ASSERT_EQUAL(BI_compare(first, expected), BI_EQUAL);

    BI_free(first);
    BI_free(second);
    BI_free(copy);
    BI_free(expected);
}

void test_Nominal_LargeInts_InPlace(void)
{
    BigInt* temp   = BI_construct(1e6);
    BigInt* first  = BI_construct(0);
    BigInt* second = BI_construct(30);

    BI_power(first, temp, 30U);

    BigInt* copy = BI_construct(0);
    BI_copy(copy, first);

    BigInt* expected = BI_construct(10U);

    BI_modBigIntIP(first, second);

    TEST_ASSERT_EQUAL(BI_compare(copy, first), BI_GREATER);
    TEST_ASSERT_EQUAL(BI_compare(first, expected), BI_EQUAL);

    BI_free(temp);
    BI_free(first);
    BI_free(second);
    BI_free(copy);
    BI_free(expected);
}

void test_OffNominal_SmallInts_InPlace_NoChange(void)
{
    BigInt* first  = BI_construct(30);
    BigInt* second = BI_construct(1e6);

    BigInt* copy = BI_construct(0);
    BI_copy(copy, first);

    BigInt* expected = BI_construct(30);

    BI_modBigIntIP(first, second);

    TEST_ASSERT_EQUAL(BI_compare(copy, first), BI_EQUAL);
    TEST_ASSERT_EQUAL(BI_compare(first, expected), BI_EQUAL);

    BI_free(first);
    BI_free(second);
    BI_free(copy);
    BI_free(expected);
}

void runModuloTests(void)
{
    RUN_TEST(test_Nominal_LargeInts_InPlace);
    RUN_TEST(test_Nominal_SmallInts_InPlace);
    RUN_TEST(test_OffNominal_SmallInts_InPlace_NoChange);
}
