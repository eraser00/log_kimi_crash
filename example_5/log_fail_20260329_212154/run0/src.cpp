#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "co_routine.h"

// Test counter to track execution
static int test_routine_executed = 0;
static int test_arg_passed = 0;

// Simple test routine for co_create and co_resume
void* test_routine(void* arg)
{
    int value = (int)(long)arg;
    test_arg_passed = value;
    test_routine_executed = 1;
    printf("Test routine executed with arg: %d\n", value);

    // Yield to test co_yield and co_resume again
    co_yield_ct();

    printf("Test routine resumed after yield\n");
    test_routine_executed = 2;

    return NULL;
}

// Test routine for multiple coroutines
void* multi_co_routine(void* arg)
{
    int id = (int)(long)arg;
    printf("Coroutine %d started\n", id);

    for (int i = 0; i < 3; i++) {
        printf("Coroutine %d iteration %d\n", id, i);
        co_yield_ct();
    }

    printf("Coroutine %d finished\n", id);
    return NULL;
}

int main(int argc, char* argv[])
{
    printf("=== Testing co_create and co_resume ===\n");

    // Test 1: Basic co_create and co_resume
    printf("\n--- Test 1: Basic co_create and co_resume ---\n");
    stCoRoutine_t* co1 = NULL;
    int ret = co_create(&co1, NULL, test_routine, (void*)(long)42);
    if (ret != 0) {
        printf("ERROR: co_create failed with ret=%d\n", ret);
        return 1;
    }
    printf("Created coroutine co1\n");

    // Resume the coroutine
    printf("Resuming co1 (first time)...\n");
    co_resume(co1);

    // Check if routine executed
    if (!test_routine_executed) {
        printf("ERROR: Test routine was not executed!\n");
        return 1;
    }
    if (test_arg_passed != 42) {
        printf("ERROR: Arg was not passed correctly! Expected 42, got %d\n", test_arg_passed);
        return 1;
    }
    printf("co1 executed successfully with arg=42\n");

    // Resume again after yield
    printf("Resuming co1 (after yield)...\n");
    co_resume(co1);

    if (test_routine_executed != 2) {
        printf("ERROR: Test routine was not resumed correctly!\n");
        return 1;
    }
    printf("co1 resumed successfully after yield\n");

    // Test 2: Create new co_routine after first one completes
    printf("\n--- Test 2: Create new co_routine ---\n");
    stCoRoutine_t* co2 = NULL;
    test_routine_executed = 0;
    ret = co_create(&co2, NULL, test_routine, (void*)(long)100);
    if (ret != 0) {
        printf("ERROR: co_create failed for co2\n");
        return 1;
    }
    printf("Created new coroutine co2\n");

    co_resume(co2);
    if (test_arg_passed != 100) {
        printf("ERROR: Arg was not passed correctly to co2!\n");
        return 1;
    }
    printf("co2 executed successfully with arg=100\n");

    co_resume(co2);
    printf("co2 resumed successfully after yield\n");

    // Test 3: Multiple coroutines
    printf("\n--- Test 3: Multiple coroutines ---\n");
    stCoRoutine_t* co3 = NULL;
    stCoRoutine_t* co4 = NULL;

    co_create(&co3, NULL, multi_co_routine, (void*)(long)1);
    co_create(&co4, NULL, multi_co_routine, (void*)(long)2);

    printf("Created two coroutines co3 and co4\n");

    // Interleave execution
    for (int i = 0; i < 4; i++) {
        printf("\n--- Round %d ---\n", i);
        co_resume(co3);
        co_resume(co4);
    }

    printf("\n--- All tests passed! ---\n");

    // Cleanup
    co_release(co1);
    co_release(co2);
    co_release(co3);
    co_release(co4);

    printf("Cleanup complete.\n");

    return 0;
}
