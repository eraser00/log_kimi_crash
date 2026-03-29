#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "co_routine.h"

static int g_coroutine_executed = 0;
static int g_coroutine_result = 0;

void* test_coroutine_func(void* arg)
{
    int value = *(int*)arg;
    g_coroutine_executed = 1;
    g_coroutine_result = value * 2;
    printf("Coroutine executed with arg=%d, result=%d\n", value, g_coroutine_result);
    return NULL;
}

int main(int argc, char* argv[])
{
    printf("Starting co_resume unit test\n");

    stCoRoutine_t* co = NULL;
    int arg = 42;

    printf("Test 1: Creating coroutine...\n");
    int ret = co_create(&co, NULL, test_coroutine_func, &arg);
    if (ret != 0 || co == NULL) {
        printf("FAILED: co_create failed with ret=%d\n", ret);
        return 1;
    }
    printf("PASSED: Coroutine created successfully\n");

    printf("Test 2: Resuming coroutine...\n");
    g_coroutine_executed = 0;
    co_resume(co);
    if (!g_coroutine_executed) {
        printf("FAILED: Coroutine did not execute after co_resume\n");
        return 1;
    }
    if (g_coroutine_result != 84) {
        printf("FAILED: Coroutine result=%d, expected=84\n", g_coroutine_result);
        return 1;
    }
    printf("PASSED: Coroutine resumed and executed correctly\n");

    printf("Test 3: Creating and resuming second coroutine...\n");
    stCoRoutine_t* co2 = NULL;
    int arg2 = 10;
    ret = co_create(&co2, NULL, test_coroutine_func, &arg2);
    if (ret != 0) {
        printf("FAILED: Second co_create failed\n");
        return 1;
    }
    g_coroutine_executed = 0;
    g_coroutine_result = 0;
    co_resume(co2);
    if (!g_coroutine_executed || g_coroutine_result != 20) {
        printf("FAILED: Second coroutine execution failed\n");
        return 1;
    }
    printf("PASSED: Second coroutine works correctly\n");

    printf("Cleaning up...\n");
    co_release(co);
    co_release(co2);

    printf("\n========================================\n");
    printf("All co_resume unit tests PASSED!\n");
    printf("========================================\n");

    return 0;
}
