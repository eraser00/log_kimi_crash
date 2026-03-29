#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "co_routine.h"

// Test flags
int test_create_completed = 0;
int test_resume_completed = 0;
int test_routine_executed = 0;

// Simple coroutine routine
void* test_routine(void* arg) {
    int* val = (int*)arg;
    *val = 42;
    test_routine_executed = 1;
    co_yield_ct();
    *val = 100;
    return NULL;
}

// Test co_create and co_resume
int main(int argc, char** argv) {
    stCoRoutine_t* co = NULL;
    int value = 0;

    // Test 1: Create a new coroutine
    int ret = co_create(&co, NULL, test_routine, &value);
    if (ret != 0 || co == NULL) {
        printf("FAILED: co_create failed with ret=%d\n", ret);
        return 1;
    }
    test_create_completed = 1;
    printf("PASSED: co_create successfully created a coroutine\n");

    // Test 2: Resume the coroutine (first time - starts execution)
    co_resume(co);
    if (value != 42 || !test_routine_executed) {
        printf("FAILED: co_resume first call failed, value=%d, executed=%d\n", value, test_routine_executed);
        return 1;
    }
    printf("PASSED: co_resume first execution successful, value=%d\n", value);

    // Test 3: Resume the coroutine (second time - after yield)
    co_resume(co);
    if (value != 100) {
        printf("FAILED: co_resume second call failed, value=%d\n", value);
        return 1;
    }
    printf("PASSED: co_resume second execution successful, value=%d\n", value);

    // Test 4: Create another coroutine to test co_routine creation
    stCoRoutine_t* co2 = NULL;
    int value2 = 0;
    ret = co_create(&co2, NULL, test_routine, &value2);
    if (ret != 0 || co2 == NULL) {
        printf("FAILED: co_create second routine failed\n");
        return 1;
    }
    co_resume(co2);
    co_resume(co2);
    if (value2 != 100) {
        printf("FAILED: second routine execution failed\n");
        return 1;
    }
    printf("PASSED: Multiple co_create and co_resume work correctly\n");

    // Cleanup
    co_release(co);
    co_release(co2);

    test_resume_completed = 1;
    printf("\nAll tests PASSED!\n");
    printf("- co_create: OK\n");
    printf("- co_resume: OK\n");
    printf("- co_routine creation: OK\n");

    return 0;
}
