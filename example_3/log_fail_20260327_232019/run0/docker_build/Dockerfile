FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    lld \
    make \
    cmake \
    g++ \
    git \
    python3 \
    && rm -rf /var/lib/apt/lists/*

ENV CC=clang
ENV CXX=clang++
ENV AR=llvm-ar
ENV RANLIB=llvm-ranlib

WORKDIR /workspace

COPY libco /workspace/libco
COPY docker_build/ut_co_resume.cpp /workspace/libco/install/ut_co_resume.cpp

WORKDIR /workspace/libco

RUN mkdir -p install && \
    clang++ -c -g -O0 -fPIC \
    -fprofile-instr-generate -fcoverage-mapping \
    -D_GNU_SOURCE -D_REENTRANT -Wall -pipe \
    co_epoll.cpp \
    co_routine.cpp \
    co_hook_sys_call.cpp \
    coctx.cpp \
    co_comm.cpp \
    -I. && \
    clang -c -g -O0 -fPIC \
    -D_GNU_SOURCE -D_REENTRANT -Wall -pipe \
    coctx_swap.S && \
    llvm-ar rcs install/libco.a \
    co_epoll.o \
    co_routine.o \
    co_hook_sys_call.o \
    coctx.o \
    co_comm.o \
    coctx_swap.o

RUN clang++ -o install/ut_co_resume \
    install/ut_co_resume.cpp \
    -I. \
    install/libco.a \
    -fprofile-instr-generate -fcoverage-mapping \
    -lpthread -ldl

RUN cp co_routine.h install/

WORKDIR /workspace/libco

CMD ["/bin/bash"]
