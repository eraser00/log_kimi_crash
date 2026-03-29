#include "co_routine.h"
#include <stdio.h>
#include <assert.h>

static int g_flag = 0;
static int g_count = 0;

void *test_routine(void *arg)
{
    int *val = (int*)arg;
    *val = 1;
    g_count++;
    co_yield_ct();
    *val = 2;
    g_flag = 1;
    g_count++;
    return NULL;
}

int main()
{
    stCoRoutine_t *co = NULL;
    int val = 0;
    int ret = co_create(&co, NULL, test_routine, &val);
    assert(ret == 0);
    assert(co != NULL);

    co_resume(co);
    assert(val == 1);
    assert(g_flag == 0);
    assert(g_count == 1);

    co_resume(co);
    assert(val == 2);
    assert(g_flag == 1);
    assert(g_count == 2);

    stCoRoutine_t *co2 = NULL;
    int val2 = 0;
    ret = co_create(&co2, NULL, test_routine, &val2);
    assert(ret == 0);
    co_resume(co2);
    assert(val2 == 1);
    co_resume(co2);
    assert(val2 == 2);

    co_release(co);
    co_release(co2);

    printf("ut_co_resume passed\n");
    return 0;
}
