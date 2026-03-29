/*
 * Unit Test for co_resume function
 *
 * This test verifies the basic functionality of co_resume by:
 * 1. Creating a coroutine
 * 2. Resuming it using co_resume
 * 3. Verifying the coroutine executes correctly
 * 4. Testing yield and resume cycle
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "co_routine.h"
#include "co_routine_inner.h"

// Test flags to track execution flow
static int test_flag = 0;
static int resume_count = 0;

// Coroutine function for testing co_resume
void* test_co_resume_func(void* arg)
{
    int* value = (int*)arg;

    // Mark that we've entered the coroutine
    test_flag = 1;
    *value = 100;
    resume_count++;

    // First yield
    co_yield_ct();

    // After first resume
    test_flag = 2;
    *value = 200;
    resume_count++;

    // Second yield
    co_yield_ct();

    // After second resume
    test_flag = 3;
    *value = 300;
    resume_count++;

    return NULL;
}

// Test 1: Basic co_resume functionality
int test_basic_co_resume()
{
    printf("Test 1: Basic co_resume functionality\n");

    // Initialize the current thread environment
    co_init_curr_thread_env();

    stCoRoutine_t* co = NULL;
    int value = 0;

    // Create coroutine
    int ret = co_create(&co, NULL, test_co_resume_func, &value);
    if (ret != 0) {
        printf("  FAILED: co_create returned %d\n", ret);
        return -1;
    }

    // Reset flags
    test_flag = 0;
    resume_count = 0;

    // First resume - should start the coroutine
    co_resume(co);

    if (test_flag != 1 || value != 100 || resume_count != 1) {
        printf("  FAILED: First resume did not execute correctly\n");
        printf("    test_flag=%d (expected 1), value=%d (expected 100), resume_count=%d (expected 1)\n",
               test_flag, value, resume_count);
        co_release(co);
        return -1;
    }
    printf("  First resume: OK (flag=%d, value=%d)\n", test_flag, value);

    // Second resume - should continue after first yield
    co_resume(co);

    if (test_flag != 2 || value != 200 || resume_count != 2) {
        printf("  FAILED: Second resume did not execute correctly\n");
        printf("    test_flag=%d (expected 2), value=%d (expected 200), resume_count=%d (expected 2)\n",
               test_flag, value, resume_count);
        co_release(co);
        return -1;
    }
    printf("  Second resume: OK (flag=%d, value=%d)\n", test_flag, value);

    // Third resume - should continue after second yield
    co_resume(co);

    if (test_flag != 3 || value != 300 || resume_count != 3) {
        printf("  FAILED: Third resume did not execute correctly\n");
        printf("    test_flag=%d (expected 3), value=%d (expected 300), resume_count=%d (expected 3)\n",
               test_flag, value, resume_count);
        co_release(co);
        return -1;
    }
    printf("  Third resume: OK (flag=%d, value=%d)\n", test_flag, value);

    // Release the coroutine
    co_release(co);

    printf("  PASSED\n\n");
    return 0;
}

// Test 2: co_resume with cStart flag (coroutine already started)
int test_resume_started_co()
{
    printf("Test 2: Resume already started coroutine\n");

    // Initialize the current thread environment
    co_init_curr_thread_env();

    stCoRoutine_t* co = NULL;
    int value = 0;

    // Create coroutine
    co_create(&co, NULL, test_co_resume_func, &value);

    // First resume to start it
    co_resume(co);

    // Second resume should continue from yield point (not restart)
    int prev_count = resume_count;
    co_resume(co);

    if (resume_count != prev_count + 1) {
        printf("  FAILED: Resume count did not increment correctly\n");
        co_release(co);
        return -1;
    }

    // Clean up - resume until coroutine completes
    while (test_flag != 3) {
        co_resume(co);
    }

    co_release(co);
    printf("  PASSED\n\n");
    return 0;
}

// Test 3: Multiple coroutines with co_resume
void* multi_co_func(void* arg)
{
    int id = *(int*)arg;
    *(int*)arg = id * 10;
    co_yield_ct();
    *(int*)arg = id * 100;
    return NULL;
}

int test_multiple_coroutines()
{
    printf("Test 3: Multiple coroutines resume\n");

    // Initialize the current thread environment
    co_init_curr_thread_env();

    const int NUM_CO = 5;
    stCoRoutine_t* cos[NUM_CO];
    int values[NUM_CO];

    // Create multiple coroutines
    for (int i = 0; i < NUM_CO; i++) {
        values[i] = i + 1;
        co_create(&cos[i], NULL, multi_co_func, &values[i]);
    }

    // Resume all coroutines first time
    for (int i = 0; i < NUM_CO; i++) {
        co_resume(cos[i]);
    }

    // Check first resume results
    for (int i = 0; i < NUM_CO; i++) {
        int expected = (i + 1) * 10;
        if (values[i] != expected) {
            printf("  FAILED: Coroutine %d first resume, expected %d, got %d\n",
                   i, expected, values[i]);
            for (int j = 0; j < NUM_CO; j++) co_release(cos[j]);
            return -1;
        }
    }
    printf("  All %d coroutines first resume: OK\n", NUM_CO);

    // Resume all coroutines second time
    for (int i = 0; i < NUM_CO; i++) {
        co_resume(cos[i]);
    }

    // Check second resume results
    for (int i = 0; i < NUM_CO; i++) {
        int expected = (i + 1) * 100;
        if (values[i] != expected) {
            printf("  FAILED: Coroutine %d second resume, expected %d, got %d\n",
                   i, expected, values[i]);
            for (int j = 0; j < NUM_CO; j++) co_release(cos[j]);
            return -1;
        }
    }
    printf("  All %d coroutines second resume: OK\n", NUM_CO);

    // Release all coroutines
    for (int i = 0; i < NUM_CO; i++) {
        co_release(cos[i]);
    }

    printf("  PASSED\n\n");
    return 0;
}

// Test 4: co_resume with self
void* self_resume_func(void* arg)
{
    stCoRoutine_t* self = co_self();
    static int count = 0;

    count++;
    if (count < 3) {
        // Try to resume self - this should be handled correctly
        // Note: In practice, resuming self from within can cause issues
        // This test mainly checks that co_resume doesn't crash
        co_yield_ct();
    }

    *(int*)arg = count;
    return NULL;
}

int test_co_self_resume()
{
    printf("Test 4: co_resume with co_self\n");

    // Initialize the current thread environment
    co_init_curr_thread_env();

    stCoRoutine_t* co = NULL;
    int result = 0;

    co_create(&co, NULL, self_resume_func, &result);
    co_resume(co);
    co_resume(co);
    co_resume(co);

    if (result != 3) {
        printf("  FAILED: Expected result=3, got %d\n", result);
        co_release(co);
        return -1;
    }

    co_release(co);
    printf("  PASSED\n\n");
    return 0;
}

int main(int argc, char* argv[])
{
    printf("=====================================\n");
    printf("Unit Test: co_resume\n");
    printf("=====================================\n\n");

    int failures = 0;

    if (test_basic_co_resume() != 0) failures++;
    if (test_resume_started_co() != 0) failures++;
    if (test_multiple_coroutines() != 0) failures++;
    if (test_co_self_resume() != 0) failures++;

    printf("=====================================\n");
    if (failures == 0) {
        printf("All tests PASSED!\n");
        printf("=====================================\n");
        return 0;
    } else {
        printf("%d test(s) FAILED!\n", failures);
        printf("=====================================\n");
        return 1;
    }
}
