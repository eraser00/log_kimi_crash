#include <stdio.h>
#include <stdlib.h>
#include "../co_routine.h"

static int test_counter = 0;

void* test_coroutine(void* arg)
{
    test_counter++;
    printf("In coroutine: test_counter = %d\n", test_counter);

    co_yield_ct();

    test_counter++;
    printf("Back in coroutine: test_counter = %d\n", test_counter);

    return NULL;
}

int main(int argc, char** argv)
{
    printf("Starting co_resume unit test\n");

    stCoRoutine_t* co = NULL;

    int ret = co_create(&co, NULL, test_coroutine, NULL);
    if (ret != 0) {
        printf("FAILED: co_create returned %d\n", ret);
        return 1;
    }
    printf("Coroutine created successfully\n");

    printf("Calling co_resume (first time)...\n");
    co_resume(co);

    if (test_counter != 1) {
        printf("FAILED: Expected test_counter=1 after first resume, got %d\n", test_counter);
        return 1;
    }
    printf("First co_resume succeeded: test_counter = %d\n", test_counter);

    printf("Calling co_resume (second time)...\n");
    co_resume(co);

    if (test_counter != 2) {
        printf("FAILED: Expected test_counter=2 after second resume, got %d\n", test_counter);
        return 1;
    }
    printf("Second co_resume succeeded: test_counter = %d\n", test_counter);

    co_release(co);
    printf("Coroutine released successfully\n");

    printf("\n=== co_resume unit test PASSED ===\n");
    return 0;
}
