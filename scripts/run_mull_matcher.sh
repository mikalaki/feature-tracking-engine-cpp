#!/usr/bin/env bash
set -euo pipefail

# Manual helper for mutation testing the matcher-related executable.
# Assumes Mull/LLVM 19 packages are installed.

BUILD_DIR="${1:-build-mull}"

cmake -S . -B "${BUILD_DIR}" \
  -DCMAKE_BUILD_TYPE=Debug \
  -DCMAKE_CXX_COMPILER=clang++ \
  -DBUILD_TESTING=ON \
  -DCMAKE_CXX_FLAGS="-fpass-plugin=/usr/lib/mull-ir-frontend-19 -g -grecord-command-line"

cmake --build "${BUILD_DIR}" --target nearest_neighbor_matcher_tests --parallel

mull-runner -ide-reporter-show-killed "${BUILD_DIR}/tests/nearest_neighbor_matcher_tests"
