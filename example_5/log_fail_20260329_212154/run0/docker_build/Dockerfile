# Use Ubuntu 22.04 as base image
FROM ubuntu:22.04

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV CC=clang
ENV CXX=clang++
ENV LLVM_PROFILE_FILE=/workspace/libco/coverage/%p.profraw

# Install dependencies
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    make \
    cmake \
    git \
    python3 \
    libc++-dev \
    libc++abi-dev \
    && rm -rf /var/lib/apt/lists/*

# Create workspace directory
WORKDIR /workspace

# Copy libco source to /workspace/libco
COPY libco /workspace/libco

# Set working directory to libco
WORKDIR /workspace/libco

# Create install and coverage directories
RUN mkdir -p /workspace/libco/install /workspace/libco/coverage

# Build libco with coverage using clang
# Compile source files with coverage flags
RUN clang++ -c -g -O0 -fprofile-instr-generate -fcoverage-mapping \
    -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -m64 \
    co_routine.cpp -o co_routine.o

RUN clang++ -c -g -O0 -fprofile-instr-generate -fcoverage-mapping \
    -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -m64 \
    co_epoll.cpp -o co_epoll.o

RUN clang++ -c -g -O0 -fprofile-instr-generate -fcoverage-mapping \
    -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -m64 \
    co_hook_sys_call.cpp -o co_hook_sys_call.o

RUN clang++ -c -g -O0 -fprofile-instr-generate -fcoverage-mapping \
    -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -m64 \
    coctx.cpp -o coctx.o

# Compile assembly file
RUN clang -c -g -O0 -fPIC -m64 \
    coctx_swap.S -o coctx_swap.o

# Create static library
RUN ar rcs /workspace/libco/install/libco.a \
    co_routine.o co_epoll.o co_hook_sys_call.o coctx.o coctx_swap.o

# Copy the unit test source file from docker_build folder (build context is project root)
COPY docker_build/ut_co_resume.cpp /workspace/libco/ut_co_resume.cpp

# Build the unit test with coverage
RUN clang++ -g -O0 -fprofile-instr-generate -fcoverage-mapping \
    -fno-strict-aliasing -Wall -pipe -D_GNU_SOURCE -D_REENTRANT -fPIC -m64 \
    -I/workspace/libco \
    /workspace/libco/ut_co_resume.cpp \
    /workspace/libco/install/libco.a \
    -lpthread -ldl \
    -o /workspace/libco/install/ut_co_resume

# Copy the coverage script
COPY docker_build/generate_coverage.sh /workspace/libco/generate_coverage.sh
RUN chmod +x /workspace/libco/generate_coverage.sh

# Set the default command to run the unit test and generate coverage
CMD ["/workspace/libco/generate_coverage.sh"]
