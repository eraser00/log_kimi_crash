#!/bin/bash
set -e

# 编译脚本：使用clang编译带覆盖率支持的libco

cd /workspace/libco

# 创建必要的目录
mkdir -p lib
mkdir -p install

# 覆盖率编译选项（clang原生支持）
COVERAGE_FLAGS="-fprofile-instr-generate -fcoverage-mapping"

# 基础编译选项
BASE_FLAGS="-g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64"

# 编译源文件为对象文件（带覆盖率）
clang++ $BASE_FLAGS $COVERAGE_FLAGS \
    -c co_epoll.cpp -o co_epoll.o

clang++ $BASE_FLAGS $COVERAGE_FLAGS \
    -c co_routine.cpp -o co_routine.o

clang++ $BASE_FLAGS $COVERAGE_FLAGS \
    -c co_hook_sys_call.cpp -o co_hook_sys_call.o

clang++ $BASE_FLAGS $COVERAGE_FLAGS \
    -c coctx.cpp -o coctx.o

clang++ $BASE_FLAGS $COVERAGE_FLAGS \
    -c co_comm.cpp -o co_comm.o

# 编译汇编文件（不需要覆盖率）
clang $BASE_FLAGS \
    -c coctx_swap.S -o coctx_swap.o

# 创建静态库
llvm-ar rcs libco.a co_epoll.o co_routine.o co_hook_sys_call.o coctx.o co_comm.o coctx_swap.o

# 复制到install目录
cp libco.a install/libco.a

echo "libco.a built successfully at /workspace/libco/install/libco.a"
