#include <stdio.h>
#include <stdlib.h>
#include "co_routine.h"

/* Global counter to track execution */
static int g_exec_count = 0;
static int g_resume_count = 0;

/* Coroutine function that yields and resumes */
void* co_test_func(void* arg) {
    g_exec_count++;
    printf("Coroutine executing, count=%d\n", g_exec_count);

    /* Yield to main */
    co_yield_ct();
    g_resume_count++;
    printf("Coroutine resumed after first yield, resume_count=%d\n", g_resume_count);

    g_exec_count++;
    printf("Coroutine executing again, count=%d\n", g_exec_count);

    /* Yield again */
    co_yield_ct();
    g_resume_count++;
    printf("Coroutine resumed after second yield, resume_count=%d\n", g_resume_count);

    g_exec_count++;
    printf("Coroutine final execution, count=%d\n", g_exec_count);

    return NULL;
}

int main() {
    printf("=== co_resume Unit Test ===\n");

    stCoRoutine_t* co = NULL;
    stCoRoutineAttr_t attr;

    /* Create a coroutine */
    int ret = co_create(&co, &attr, co_test_func, NULL);
    if (ret != 0 || co == NULL) {
        printf("FAIL: Failed to create coroutine\n");
        return 1;
    }
    printf("Coroutine created successfully\n");

    /* First resume: starts the coroutine */
    printf("\n--- First co_resume (start coroutine) ---\n");
    co_resume(co);
    if (g_exec_count != 1) {
        printf("FAIL: Expected exec_count=1 after first resume, got %d\n", g_exec_count);
        return 1;
    }
    printf("First resume successful, coroutine yielded\n");

    /* Second resume: resumes from first yield */
    printf("\n--- Second co_resume (resume from first yield) ---\n");
    co_resume(co);
    if (g_exec_count != 2 || g_resume_count != 1) {
        printf("FAIL: Expected exec_count=2, resume_count=1 after second resume, got exec=%d, resume=%d\n",
               g_exec_count, g_resume_count);
        return 1;
    }
    printf("Second resume successful, coroutine yielded again\n");

    /* Third resume: resumes from second yield and completes */
    printf("\n--- Third co_resume (resume from second yield and complete) ---\n");
    co_resume(co);
    if (g_exec_count != 3 || g_resume_count != 2) {
        printf("FAIL: Expected exec_count=3, resume_count=2 after third resume, got exec=%d, resume=%d\n",
               g_exec_count, g_resume_count);
        return 1;
    }
    printf("Third resume successful, coroutine completed\n");

    /* Release the coroutine */
    co_release(co);
    printf("\nCoroutine released\n");

    /* Test multiple coroutines */
    printf("\n=== Test multiple coroutines ===\n");
    stCoRoutine_t* co1 = NULL;
    stCoRoutine_t* co2 = NULL;

    ret = co_create(&co1, &attr, co_test_func, NULL);
    if (ret != 0) {
        printf("FAIL: Failed to create coroutine 1\n");
        return 1;
    }

    ret = co_create(&co2, &attr, co_test_func, NULL);
    if (ret != 0) {
        printf("FAIL: Failed to create coroutine 2\n");
        return 1;
    }

    /* Alternate between coroutines */
    printf("\n--- Alternating between coroutines ---\n");
    co_resume(co1);
    co_resume(co2);
    co_resume(co1);
    co_resume(co2);
    co_resume(co1);
    co_resume(co2);

    co_release(co1);
    co_release(co2);

    printf("\n=== All co_resume tests PASSED ===\n");
    return 0;
}
