#include "co_routine.h"
#include <stdio.h>
#include <assert.h>

static int g_step = 0;

static void* test_routine(void* arg)
{
    g_step = 1;
    co_yield_ct();
    g_step = 2;
    return NULL;
}

int main()
{
    stCoRoutine_t* co = NULL;
    int ret = co_create(&co, NULL, test_routine, NULL);
    assert(ret == 0);
    assert(co != NULL);

    assert(g_step == 0);
    co_resume(co);
    assert(g_step == 1);

    co_resume(co);
    assert(g_step == 2);

    co_release(co);

    printf("ut_co_resume passed\n");
    return 0;
}
