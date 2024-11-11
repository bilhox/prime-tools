#include <stdio.h>

#include "bigint.h"
#include "unity.h"

#include "testModulo.h"

void setUp    (void)
{
    if (BI_init() < 0)
    {
        printf("BI_init failed\n");
        exit(EXIT_FAILURE);
    }
}

void tearDown (void)
{
    if (BI_quit() < 0)
    {
        printf("BI_quit failed\n");
        exit(EXIT_FAILURE);
    }
}

int main(void)
{
    UNITY_BEGIN();

    runModuloTests();

    return UNITY_END();
}