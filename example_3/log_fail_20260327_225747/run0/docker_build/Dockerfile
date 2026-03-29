FROM ubuntu:22.04

# 安装必要的依赖
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    make \
    g++ \
    libc++-dev \
    libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

# 设置工作目录
WORKDIR /workspace

# 创建libco源码目录
RUN mkdir -p /workspace/libco

# 设置环境变量使用clang
ENV CC=clang
ENV CXX=clang++
ENV AR=llvm-ar
ENV RANLIB=llvm-ranlib

# 复制libco源码到容器中
COPY libco /workspace/libco/

# 复制单元测试源文件
COPY docker_build/ut_co_resume.cpp /workspace/libco/install/ut_co_resume.cpp

# 编译覆盖率版本的libco
WORKDIR /workspace/libco

# 创建安装目录
RUN mkdir -p /workspace/libco/install

# 使用clang编译，添加覆盖率选项
# 编译所有需要的源文件
RUN clang++ -c -g -fPIC -O0 -Wall -fprofile-instr-generate -fcoverage-mapping \
    -D_GNU_SOURCE -D_REENTRANT \
    co_epoll.cpp -o co_epoll.o && \
    clang++ -c -g -fPIC -O0 -Wall -fprofile-instr-generate -fcoverage-mapping \
    -D_GNU_SOURCE -D_REENTRANT \
    co_routine.cpp -o co_routine.o && \
    clang++ -c -g -fPIC -O0 -Wall -fprofile-instr-generate -fcoverage-mapping \
    -D_GNU_SOURCE -D_REENTRANT \
    co_hook_sys_call.cpp -o co_hook_sys_call.o && \
    clang++ -c -g -fPIC -O0 -Wall -fprofile-instr-generate -fcoverage-mapping \
    -D_GNU_SOURCE -D_REENTRANT \
    coctx.cpp -o coctx.o && \
    clang -c -g -fPIC -O0 -Wall \
    -D_GNU_SOURCE -D_REENTRANT \
    coctx_swap.S -o coctx_swap.o && \
    clang++ -c -g -fPIC -O0 -Wall -fprofile-instr-generate -fcoverage-mapping \
    -D_GNU_SOURCE -D_REENTRANT \
    co_comm.cpp -o co_comm.o

# 创建静态库
RUN llvm-ar rcs /workspace/libco/install/libco.a \
    co_epoll.o co_routine.o co_hook_sys_call.o \
    coctx.o coctx_swap.o co_comm.o

# 编译单元测试
RUN clang++ -g -fPIC -O0 -Wall -fprofile-instr-generate -fcoverage-mapping \
    -D_GNU_SOURCE -D_REENTRANT \
    -I/workspace/libco \
    -o /workspace/libco/install/ut_co_resume \
    /workspace/libco/install/ut_co_resume.cpp \
    /workspace/libco/install/libco.a \
    -lpthread -ldl

# 验证文件存在
RUN ls -la /workspace/libco/install/

WORKDIR /workspace
