#!/bin/bash
set -e

cd /workspace/libco

# Run the unit test (generates .profraw files)
LLVM_PROFILE_FILE="ut_co_resume.profraw" ./install/ut_co_resume

# Merge raw profile data
llvm-profdata merge -sparse ut_co_resume.profraw -o ut_co_resume.profdata

# Generate coverage report
llvm-cov show ./install/ut_co_resume -instr-profile=ut_co_resume.profdata \
    > /workspace/libco/install/ut_co_resume.cov.txt

# Also generate a summary
llvm-cov report ./install/ut_co_resume -instr-profile=ut_co_resume.profdata

echo "Coverage report generated at: /workspace/libco/install/ut_co_resume.cov.txt"
