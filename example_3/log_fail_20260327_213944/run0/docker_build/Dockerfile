# 使用Ubuntu 22.04作为基础镜像
FROM ubuntu:22.04

# 设置环境变量
ENV DEBIAN_FRONTEND=noninteractive
ENV CC=clang
ENV CXX=clang++

# 安装必要的依赖
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    lld \
    make \
    g++ \
    git \
    curl \
    wget \
    vim \
    libc++-dev \
    libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

# 创建工作目录
WORKDIR /workspace

# 复制libco源码到镜像中
COPY libco /workspace/libco

# 设置工作目录为libco
WORKDIR /workspace/libco

# 修改Makefile以支持clang和覆盖率编译
RUN sed -i 's/CC = gcc/CC = clang/' co.mk && \
    sed -i 's/CXX = g++/CXX = clang++/' co.mk && \
    sed -i 's/CFLAGS= -g \$(INCLS)/CFLAGS= -g -O0 --coverage -fprofile-instr-generate -fcoverage-mapping $(INCLS)/' co.mk && \
    sed -i 's/CFLAGS= -O2 \$(INCLS)/CFLAGS= -O0 --coverage -fprofile-instr-generate -fcoverage-mapping $(INCLS)/' co.mk

# 编译静态库
RUN make clean 2>/dev/null; make libcolib.a

# 创建install目录并复制库文件
RUN mkdir -p /workspace/libco/install && \
    cp /workspace/libco/lib/libcolib.a /workspace/libco/install/libco.a

# 复制单元测试源码到install目录
COPY docker_build/ut_co_resume.cpp /workspace/libco/install/ut_co_resume.cpp

# 编译单元测试 ut_co_resume
RUN clang++ -g -O0 --coverage -fprofile-instr-generate -fcoverage-mapping \
    -o /workspace/libco/install/ut_co_resume \
    /workspace/libco/install/ut_co_resume.cpp \
    -I/workspace/libco \
    /workspace/libco/install/libco.a \
    -lpthread -ldl

# 设置默认工作目录
WORKDIR /workspace/libco

# 默认命令
CMD ["/bin/bash"]
