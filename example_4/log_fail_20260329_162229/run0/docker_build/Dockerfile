# 使用ubuntu:22.04作为基础镜像
FROM ubuntu:22.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive

# 安装必要的依赖
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    lcov \
    cmake \
    make \
    g++ \
    git \
    perl \
    && rm -rf /var/lib/apt/lists/*

# 创建工作目录
WORKDIR /workspace

# 复制libco源码到镜像中
COPY libco /workspace/libco

# 复制构建脚本
COPY docker_build/build_libco.sh /workspace/libco/build_libco.sh
RUN chmod +x /workspace/libco/build_libco.sh

# 设置工作目录为libco
WORKDIR /workspace/libco

# 创建安装目录
RUN mkdir -p /workspace/libco/install

# 先编译一个不带覆盖率的版本测试基本功能
RUN clang++ -g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c co_epoll.cpp -o co_epoll.o && \
    clang++ -g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c co_routine.cpp -o co_routine.o && \
    clang++ -g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c co_hook_sys_call.cpp -o co_hook_sys_call.o && \
    clang++ -g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c coctx.cpp -o coctx.o && \
    clang++ -g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c co_comm.cpp -o co_comm.o && \
    clang -g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c coctx_swap.S -o coctx_swap.o && \
    llvm-ar rcs /workspace/libco/install/libco.a co_epoll.o co_routine.o co_hook_sys_call.o coctx.o co_comm.o coctx_swap.o

# 编译单元测试（不带覆盖率先测试基本功能）
RUN clang++ -g -O0 -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -o /workspace/libco/install/ut_co_resume \
    /workspace/libco/ut_co_resume.cpp \
    -I/workspace/libco \
    /workspace/libco/install/libco.a \
    -lpthread -ldl

# 设置默认工作目录
WORKDIR /workspace/libco

CMD ["/bin/bash"]
