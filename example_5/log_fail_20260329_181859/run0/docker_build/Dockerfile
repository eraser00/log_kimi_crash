FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    make \
    && rm -rf /var/lib/apt/lists/*

COPY libco /workspace/libco
WORKDIR /workspace/libco

RUN mkdir -p install && \
    clang++ -fprofile-instr-generate -fcoverage-mapping \
        -g -fno-strict-aliasing -O2 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
        -c co_epoll.cpp -o co_epoll.o && \
    clang++ -fprofile-instr-generate -fcoverage-mapping \
        -g -fno-strict-aliasing -O2 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
        -c co_routine.cpp -o co_routine.o && \
    clang++ -fprofile-instr-generate -fcoverage-mapping \
        -g -fno-strict-aliasing -O2 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
        -c co_hook_sys_call.cpp -o co_hook_sys_call.o && \
    clang++ -fprofile-instr-generate -fcoverage-mapping \
        -g -fno-strict-aliasing -O2 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
        -c coctx.cpp -o coctx.o && \
    clang -fprofile-instr-generate -fcoverage-mapping \
        -g -fno-strict-aliasing -O2 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
        -c coctx_swap.S -o coctx_swap.o && \
    clang++ -fprofile-instr-generate -fcoverage-mapping \
        -g -fno-strict-aliasing -O2 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
        -c co_comm.cpp -o co_comm.o && \
    llvm-ar rcs install/libco.a co_epoll.o co_routine.o co_hook_sys_call.o coctx.o coctx_swap.o co_comm.o

RUN clang++ -fprofile-instr-generate -fcoverage-mapping \
    -g -O2 -Wall -D_GNU_SOURCE -D_REENTRANT -m64 \
    -I/workspace/libco \
    /workspace/libco/ut_co_resume.cpp \
    /workspace/libco/install/libco.a \
    -lpthread -ldl \
    -o /workspace/libco/install/ut_co_resume

RUN cd /workspace/libco && \
    LLVM_PROFILE_FILE="ut_co_resume.profraw" ./install/ut_co_resume && \
    llvm-profdata merge -sparse ut_co_resume.profraw -o ut_co_resume.profdata && \
    llvm-cov report ./install/ut_co_resume -instr-profile=ut_co_resume.profdata > install/ut_co_resume.cov.txt
