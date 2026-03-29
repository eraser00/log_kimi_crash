FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    make \
    g++ \
    lcov \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace

COPY libco /workspace/libco

WORKDIR /workspace/libco

RUN clang++ -g -O0 -fprofile-arcs -ftest-coverage \
    -fno-strict-aliasing -Wall -pipe \
    -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c co_epoll.cpp -o co_epoll.o && \
    clang++ -g -O0 -fprofile-arcs -ftest-coverage \
    -fno-strict-aliasing -Wall -pipe \
    -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c co_hook_sys_call.cpp -o co_hook_sys_call.o && \
    clang++ -g -O0 -fprofile-arcs -ftest-coverage \
    -fno-strict-aliasing -Wall -pipe \
    -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c co_routine.cpp -o co_routine.o && \
    clang++ -g -O0 -fprofile-arcs -ftest-coverage \
    -fno-strict-aliasing -Wall -pipe \
    -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c coctx.cpp -o coctx.o && \
    clang -g -O0 -fprofile-arcs -ftest-coverage \
    -fno-strict-aliasing -Wall -pipe \
    -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -c coctx_swap.S -o coctx_swap.o

RUN mkdir -p /workspace/libco/install

RUN llvm-ar rcs /workspace/libco/install/libco.a \
    co_epoll.o co_hook_sys_call.o co_routine.o coctx.o coctx_swap.o

COPY docker_build/ut_co_resume.cpp /workspace/libco/install/ut_co_resume.cpp

RUN clang++ -g -O0 -fprofile-arcs -ftest-coverage \
    -fno-strict-aliasing -Wall -pipe \
    -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -I/workspace/libco \
    /workspace/libco/install/ut_co_resume.cpp \
    /workspace/libco/install/libco.a \
    -lpthread -ldl \
    -o /workspace/libco/install/ut_co_resume

RUN rm -f *.o *.gcno

WORKDIR /workspace/libco

CMD ["/bin/bash"]
