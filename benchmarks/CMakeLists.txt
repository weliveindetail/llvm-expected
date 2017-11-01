cmake_minimum_required(VERSION 3.2)

set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "Don't include tests for benchmark itself" FORCE)

cmake_policy(PUSH)
add_subdirectory(benchmark)
cmake_policy(POP)

add_subdirectory(Exception)
add_subdirectory(LLVMExpected)
