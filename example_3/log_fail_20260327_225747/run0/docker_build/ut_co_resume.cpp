#include <stdio.h>
#include <stdlib.h>
#include "co_routine.h"

// 简单的协程函数：只执行一次
void* simple_routine(void* arg)
{
    int* value = (int*)arg;
    *value = 42;
    printf("[TEST] Simple routine executed, value = %d\n", *value);
    return NULL;
}

// 测试协程创建和恢复
int test_create_and_resume()
{
    printf("\n[TEST] Test 1: Basic co_create and co_resume\n");

    int result = 0;
    stCoRoutine_t* co = NULL;

    // 创建协程
    int ret = co_create(&co, NULL, simple_routine, &result);
    if (ret != 0 || co == NULL) {
        printf("[FAIL] co_create failed\n");
        return -1;
    }
    printf("[PASS] co_create succeeded\n");

    // 恢复协程（首次启动）
    co_resume(co);

    if (result == 42) {
        printf("[PASS] co_resume executed routine correctly (result = %d)\n", result);
    } else {
        printf("[FAIL] Routine did not execute correctly (result = %d, expected 42)\n", result);
        return -1;
    }

    co_release(co);
    return 0;
}

// 测试co_self函数
int test_co_self()
{
    printf("\n[TEST] Test 2: co_self function\n");

    stCoRoutine_t* self = co_self();
    if (self != NULL) {
        printf("[PASS] co_self returned valid pointer: %p\n", (void*)self);
        return 0;
    } else {
        printf("[FAIL] co_self returned NULL\n");
        return -1;
    }
}

// 测试多个协程创建
int test_multiple_coroutines()
{
    printf("\n[TEST] Test 3: Multiple coroutines\n");

    int results[5] = {0};
    stCoRoutine_t* cos[5] = {NULL};

    // 创建5个协程
    for (int i = 0; i < 5; i++) {
        int ret = co_create(&cos[i], NULL, simple_routine, &results[i]);
        if (ret != 0 || cos[i] == NULL) {
            printf("[FAIL] co_create failed for coroutine %d\n", i);
            return -1;
        }
    }
    printf("[PASS] Created 5 coroutines\n");

    // 依次恢复每个协程
    for (int i = 0; i < 5; i++) {
        co_resume(cos[i]);
        if (results[i] == 42) {
            printf("[PASS] Coroutine %d executed correctly\n", i);
        } else {
            printf("[FAIL] Coroutine %d result = %d, expected 42\n", i, results[i]);
            return -1;
        }
    }

    // 释放所有协程
    for (int i = 0; i < 5; i++) {
        co_release(cos[i]);
    }

    return 0;
}

int main(int argc, char* argv[])
{
    printf("========================================\n");
    printf("       co_resume Unit Test Suite\n");
    printf("========================================\n");

    int passed = 0;
    int failed = 0;

    // 运行测试1
    if (test_create_and_resume() == 0) {
        passed++;
    } else {
        failed++;
    }

    // 运行测试2
    if (test_co_self() == 0) {
        passed++;
    } else {
        failed++;
    }

    // 运行测试3
    if (test_multiple_coroutines() == 0) {
        passed++;
    } else {
        failed++;
    }

    // 汇总
    printf("\n========================================\n");
    printf("          Test Summary\n");
    printf("========================================\n");
    printf("Total tests: %d\n", passed + failed);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", failed);
    printf("========================================\n");

    return failed > 0 ? 1 : 0;
}
