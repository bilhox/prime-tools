#include "testModulo.h"

void setUp    (void) {}
void tearDown (void) {}

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_justToSee);

    return UNITY_END();
}