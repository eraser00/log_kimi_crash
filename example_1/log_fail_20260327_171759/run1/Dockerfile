FROM xgrammar_dev

WORKDIR /workspace/xgrammar

# Set environment variables to configure scikit-build-core for coverage build
ENV SKBUILD_CMAKE_ARGS="-DXGRAMMAR_ENABLE_COVERAGE=ON;-DXGRAMMAR_BUILD_CXX_TESTS=ON"
ENV CMAKE_BUILD_TYPE=Debug

# Clean and rebuild with coverage enabled through pip/scikit-build
RUN rm -rf build \
    && pip install -e . --no-build-isolation -v

WORKDIR /workspace
