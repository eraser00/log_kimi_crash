
# 使用ubuntu:22.04作为基础镜像
FROM ubuntu:22.04

# 设置非交互式环境变量
ENV DEBIAN_FRONTEND=noninteractive

# 安装必要的依赖：clang、LLVM工具链、构建工具、git
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    lld \
    build-essential \
    cmake \
    git \
    python3 \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /workspace

# 创建libco目录
RUN mkdir -p /workspace/libco /workspace/libco/install

# 复制libco源代码到容器中
COPY libco/ /workspace/libco/

# 设置编译器为clang
ENV CC=clang
ENV CXX=clang++

# 编译覆盖率版本的libco静态库
# 使用-fprofile-instr-generate和-fcoverage-mapping启用覆盖率支持
WORKDIR /workspace/libco

# 编译所有源文件为对象文件（带覆盖率支持）
RUN clang++ -g -fno-strict-aliasing -O2 -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -fprofile-instr-generate -fcoverage-mapping \
    -I. -c \
    co_epoll.cpp \
    co_routine.cpp \
    co_hook_sys_call.cpp \
    coctx.cpp \
    coctx_swap.S \
    co_comm.cpp

# 创建静态库
RUN ar rcs /workspace/libco/install/libco.a \
    co_epoll.o \
    co_routine.o \
    co_hook_sys_call.o \
    coctx.o \
    coctx_swap.o \
    co_comm.o

# 创建ut_co_resume.cpp单元测试源文件
RUN cat > /workspace/libco/ut_co_resume.cpp << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include "co_routine.h"

// 全局计数器用于验证执行流程
static int g_call_count = 0;
static int g_nested_depth = 0;

// 嵌套协程函数
void* nested_coroutine(void* arg)
{
    printf("[Nested] 进入嵌套协程, depth=%d\n", g_nested_depth);
    g_call_count++;
    g_nested_depth++;

    stCoRoutine_t* self = co_self();

    // 第一层嵌套
    if (g_nested_depth <= 2) {
        printf("[Nested] 准备yield, depth=%d\n", g_nested_depth);
        co_yield_ct();  // 挂起当前协程
        printf("[Nested] 从yield恢复, depth=%d\n", g_nested_depth);
    }

    g_nested_depth--;
    printf("[Nested] 退出嵌套协程, depth=%d\n", g_nested_depth);
    return NULL;
}

// 主协程函数
void* main_coroutine(void* arg)
{
    printf("[Main] 进入主协程\n");
    g_call_count++;

    // 创建嵌套协程
    stCoRoutine_t* nested_co = NULL;
    co_create(&nested_co, NULL, nested_coroutine, NULL);

    printf("[Main] 准备第一次resume嵌套协程\n");
    // 第一次resume嵌套协程 - 这体现co_resume的嵌套调用
    co_resume(nested_co);

    printf("[Main] 嵌套协程yield后，主协程继续执行\n");
    g_call_count++;

    printf("[Main] 准备第二次resume嵌套协程\n");
    // 第二次resume嵌套协程
    co_resume(nested_co);

    printf("[Main] 嵌套协程执行完毕\n");
    g_call_count++;

    // 释放嵌套协程
    co_release(nested_co);

    printf("[Main] 退出主协程\n");
    return NULL;
}

int main(int argc, char* argv[])
{
    printf("=== ut_co_resume 单元测试开始 ===\n");

    // 创建主协程
    stCoRoutine_t* main_co = NULL;
    co_create(&main_co, NULL, main_coroutine, NULL);

    printf("[Test] 准备第一次resume主协程\n");
    // 第一次resume主协程
    co_resume(main_co);

    printf("[Test] 主协程yield后，测试程序继续执行\n");

    printf("[Test] 准备第二次resume主协程\n");
    // 第二次resume主协程（实际上主协程内部会完成嵌套协程的调度）
    co_resume(main_co);

    // 释放主协程
    co_release(main_co);

    printf("=== ut_co_resume 单元测试结束 ===\n");
    printf("总计调用次数: %d\n", g_call_count);

    // 验证测试结果
    if (g_call_count >= 4) {
        printf("[PASS] co_resume 嵌套调用测试通过!\n");
        return 0;
    } else {
        printf("[FAIL] co_resume 嵌套调用测试失败!\n");
        return 1;
    }
}
EOF

# 编译单元测试（带覆盖率支持）
RUN clang++ -g -fno-strict-aliasing -O2 -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -fprofile-instr-generate -fcoverage-mapping \
    -I. \
    ut_co_resume.cpp \
    /workspace/libco/install/libco.a \
    -lpthread -ldl \
    -o /workspace/libco/install/ut_co_resume

# 设置环境变量以便运行时可以找到覆盖率数据
ENV LLVM_PROFILE_FILE="/workspace/libco/ut_co_resume.profraw"

# 设置默认工作目录
WORKDIR /workspace/libco

# 验证构建结果
RUN ls -la /workspace/libco/install/ && \
    echo "=== 验证libco.a存在 ===" && \
    test -f /workspace/libco/install/libco.a && echo "libco.a 存在" || echo "libco.a 不存在" && \
    echo "=== 验证ut_co_resume存在 ===" && \
    test -f /workspace/libco/install/ut_co_resume && echo "ut_co_resume 存在" || echo "ut_co_resume 不存在"

# 默认命令：运行单元测试
CMD ["/workspace/libco/install/ut_co_resume"]
