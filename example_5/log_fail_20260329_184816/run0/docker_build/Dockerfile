FROM ubuntu:22.04

# Set non-interactive mode for apt
ENV DEBIAN_FRONTEND=noninteractive

# Install necessary packages including clang, llvm, and coverage tools
RUN apt-get update && apt-get install -y \
    clang \
    llvm \
    make \
    cmake \
    build-essential \
    lcov \
    git \
    && rm -rf /var/lib/apt/lists/*

# Set working directory
WORKDIR /workspace

# Create necessary directories
RUN mkdir -p /workspace/libco/install

# Copy libco source code
COPY libco/ /workspace/libco/

# Set environment variables for clang
ENV CC=clang
ENV CXX=clang++

# Build libco with coverage flags using clang
WORKDIR /workspace/libco

# Build static library with coverage flags
RUN clang++ -c \
    -g -fno-strict-aliasing -O0 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -fprofile-instr-generate -fcoverage-mapping \
    co_epoll.cpp co_routine.cpp co_hook_sys_call.cpp coctx.cpp coctx_swap.S

# Create static library
RUN ar -rc libco.a co_epoll.o co_routine.o co_hook_sys_call.o coctx.o coctx_swap.o

# Move to install directory
RUN cp libco.a /workspace/libco/install/

# Copy the unit test source file from local context
COPY ut_co_resume.cpp /workspace/libco/ut_co_resume.cpp

# Compile the unit test with coverage
RUN clang++ -o /workspace/libco/install/ut_co_resume \
    -g -fno-strict-aliasing -O0 -Wall -D_GNU_SOURCE -D_REENTRANT -fPIC -Wno-deprecated -m64 \
    -fprofile-instr-generate -fcoverage-mapping \
    /workspace/libco/ut_co_resume.cpp \
    /workspace/libco/install/libco.a \
    -lpthread -ldl

# Copy the coverage script
COPY run_coverage.sh /workspace/libco/install/run_coverage.sh
RUN chmod +x /workspace/libco/install/run_coverage.sh

# Run tests and generate coverage report during build
WORKDIR /workspace/libco
RUN LLVM_PROFILE_FILE="ut_co_resume.profraw" ./install/ut_co_resume && \
    llvm-profdata merge -sparse ut_co_resume.profraw -o ut_co_resume.profdata && \
    llvm-cov show ./install/ut_co_resume -instr-profile=ut_co_resume.profdata > /workspace/libco/install/ut_co_resume.cov.txt && \
    llvm-cov report ./install/ut_co_resume -instr-profile=ut_co_resume.profdata

# Set the default workdir
WORKDIR /workspace/libco

# Default command to show info
CMD ["/bin/bash", "-c", "echo 'libco_dev Docker image ready!' && echo 'Library: /workspace/libco/install/libco.a' && echo 'Unit test: /workspace/libco/install/ut_co_resume' && echo 'Test source: /workspace/libco/ut_co_resume.cpp' && echo 'To run coverage: /workspace/libco/install/run_coverage.sh'"]
