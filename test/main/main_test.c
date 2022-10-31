#include <stdio.h>
#include <string.h>
#include "unity.h"

void app_main() 
{
    UNITY_BEGIN();
    unity_run_tests_by_tag("[double]", false);
    UNITY_END();
}