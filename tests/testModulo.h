#ifndef TEST_MODULO
#define TEST_MODULO

/**
 * @brief Tests that BI_modBigIntIP works when the numbers are small
 *
 * 1_000_000 % 30 == 10
 */
void test_Nominal_SmallInts_InPlace(void);

/**
 * @brief Tests that BI_modBigIntIP works when one number is large
 *
 * (1_000_000^30) % 30 == 10
 */
void test_Nominal_LargeInts_InPlace(void);

/**
 * @brief Tests that BI_modBigIntIP works when nothing changes
 *
 * 30 % 1_000_000 == 30
 */
void test_OffNominal_SmallInts_InPlace_NoChange(void);

/**
 * @brief Runs the tests in this file. When a new test is added
 *        or a test is renamed, this function must be updated
 *        to include that change
 */
void runModuloTests(void);

#endif // ifndef TEST_MODULO