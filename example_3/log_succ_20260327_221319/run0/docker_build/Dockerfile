FROM ubuntu:22.04

# Install dependencies
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    make \
    g++ \
    libc++-dev \
    libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

# Create workspace directory
WORKDIR /workspace

# Copy libco source code
COPY libco /workspace/libco

# Copy unit test source
COPY ut_co_resume.cpp /workspace/libco/

WORKDIR /workspace/libco

# Compile libco with clang and coverage support
# Source files: co_epoll.cpp co_routine.cpp co_hook_sys_call.cpp coctx.cpp coctx_swap.S
RUN clang++ -c -g -O0 -fprofile-instr-generate -fcoverage-mapping \
    -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    co_epoll.cpp co_routine.cpp co_hook_sys_call.cpp coctx.cpp

# Compile assembly file
RUN clang -c -g -O0 -fPIC -m64 coctx_swap.S

# Create static library
RUN mkdir -p install && \
    ar rcs install/libco.a co_epoll.o co_routine.o co_hook_sys_call.o coctx.o coctx_swap.o

# Compile the unit test
RUN clang++ -g -O0 -fprofile-instr-generate -fcoverage-mapping \
    -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -o install/ut_co_resume ut_co_resume.cpp \
    install/libco.a -lpthread -ldl

# Make the test executable
RUN chmod +x install/ut_co_resume

# Verify the build
RUN ls -la install/ && echo "Build completed successfully"

CMD ["/bin/bash"]
