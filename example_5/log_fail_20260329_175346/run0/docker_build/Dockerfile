FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && apt-get install -y \
    clang llvm lld \
    cmake make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /workspace/libco

# Copy libco source
COPY libco/ /workspace/libco/

# Set clang as compiler
ENV CC=clang
ENV CXX=clang++
ENV AR=llvm-ar

# Coverage flags
ENV COV_FLAGS="-fprofile-instr-generate -fcoverage-mapping"

# Build libco static library with coverage
RUN make -j$(nproc) clean && \
    make -j$(nproc) CFLAGS="${COV_FLAGS} -g -I. -fPIC -DLINUX -pipe -c -fno-inline -fno-strict-aliasing -O2 -Wall -export-dynamic -D_GNU_SOURCE -D_REENTRANT -Wno-deprecated -m64" \
         CXXFLAGS="${COV_FLAGS} -g -I. -fPIC -DLINUX -pipe -c -fno-inline -fno-strict-aliasing -O2 -Wall -export-dynamic -D_GNU_SOURCE -D_REENTRANT -Wno-deprecated -m64" \
         CC=clang CXX=clang++ \
         LINKS="-g -L./lib -lcolib -lpthread -ldl ${COV_FLAGS}" \
         colib

# Create install directory and copy library
RUN mkdir -p /workspace/libco/install && \
    cp /workspace/libco/lib/libcolib.a /workspace/libco/install/libco.a

# Write unit test source code
RUN cat > /workspace/libco/ut_co_resume.cpp << 'UTEOF'
#include "co_routine.h"
#include <stdio.h>
#include <assert.h>

static int g_flag = 0;
static int g_count = 0;

void *test_routine(void *arg)
{
    int *val = (int*)arg;
    *val = 1;
    g_count++;
    co_yield_ct();
    *val = 2;
    g_flag = 1;
    g_count++;
    return NULL;
}

int main()
{
    stCoRoutine_t *co = NULL;
    int val = 0;
    int ret = co_create(&co, NULL, test_routine, &val);
    assert(ret == 0);
    assert(co != NULL);

    co_resume(co);
    assert(val == 1);
    assert(g_flag == 0);
    assert(g_count == 1);

    co_resume(co);
    assert(val == 2);
    assert(g_flag == 1);
    assert(g_count == 2);

    stCoRoutine_t *co2 = NULL;
    int val2 = 0;
    ret = co_create(&co2, NULL, test_routine, &val2);
    assert(ret == 0);
    co_resume(co2);
    assert(val2 == 1);
    co_resume(co2);
    assert(val2 == 2);

    co_release(co);
    co_release(co2);

    printf("ut_co_resume passed\n");
    return 0;
}
UTEOF

# Build unit test executable
RUN clang++ ${COV_FLAGS} -g -O2 -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -m64 \
    -I/workspace/libco \
    /workspace/libco/ut_co_resume.cpp \
    /workspace/libco/install/libco.a \
    -lpthread -ldl \
    -o /workspace/libco/install/ut_co_resume

# Run the unit test to generate coverage data, then generate report
RUN cd /workspace/libco && \
    LLVM_PROFILE_FILE="default.profraw" /workspace/libco/install/ut_co_resume && \
    llvm-profdata merge -sparse default.profraw -o default.profdata && \
    llvm-cov report /workspace/libco/install/ut_co_resume -instr-profile=default.profdata > /workspace/libco/install/ut_co_resume.cov.txt && \
    cat /workspace/libco/install/ut_co_resume.cov.txt
