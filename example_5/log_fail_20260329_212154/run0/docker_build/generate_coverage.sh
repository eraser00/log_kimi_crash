#!/bin/bash
set -e

# Change to the libco directory
cd /workspace/libco

# Create coverage directory if it doesn't exist
mkdir -p coverage

# Run the unit test
/workspace/libco/install/ut_co_resume

# Merge coverage data
llvm-profdata merge -sparse coverage/*.profraw -o coverage/merged.profdata

# Generate coverage report
llvm-cov report /workspace/libco/install/ut_co_resume \
    -instr-profile=coverage/merged.profdata \
    /workspace/libco/*.cpp > /workspace/libco/install/ut_co_resume.cov.txt

# Also generate detailed report
echo "" >> /workspace/libco/install/ut_co_resume.cov.txt
echo "=== Detailed Function Coverage ===" >> /workspace/libco/install/ut_co_resume.cov.txt
llvm-cov show /workspace/libco/install/ut_co_resume \
    -instr-profile=coverage/merged.profdata \
    /workspace/libco/*.cpp >> /workspace/libco/install/ut_co_resume.cov.txt

echo "Coverage report generated at: /workspace/libco/install/ut_co_resume.cov.txt"
cat /workspace/libco/install/ut_co_resume.cov.txt
