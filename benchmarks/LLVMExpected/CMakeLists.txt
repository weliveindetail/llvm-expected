cmake_minimum_required(VERSION 3.2)

add_executable(BenchmarkLLVMExpected
    BenchmarkLLVMExpected.cpp

    BenchOptimizations.cpp
    BenchOverheadExample.cpp
    BenchPerfCategories.cpp
    BenchSuccessRates.cpp

    FastRand.h
)

set_target_properties(BenchmarkLLVMExpected PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# add Google Benchmark libraries
find_package(benchmark REQUIRED)

target_include_directories(BenchmarkLLVMExpected PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(BenchmarkLLVMExpected PUBLIC 
    ${DISABLE_RTTI_FLAG}
    ${DISABLE_EXCEPTIONS_FLAG}
    ${DISABLE_STACKPROTECTOR_FLAG}
    ${DISABLE_FRAMEPOINTER_FLAG}
)

target_link_libraries(BenchmarkLLVMExpected PRIVATE
    LLVMExpected
    benchmark::benchmark
)

message("")
message("BenchmarkLLVMExpected")

get_target_property(Prop BenchmarkLLVMExpected INCLUDE_DIRECTORIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Include directories: ${Prop}")
endif()

get_target_property(Prop BenchmarkLLVMExpected LINK_LIBRARIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Link libraries: ${Prop}")
endif()

get_target_property(Prop BenchmarkLLVMExpected COMPILE_OPTIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile options: ${Prop}")
endif()

get_target_property(Prop BenchmarkLLVMExpected COMPILE_DEFINITIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile definitions: ${Prop}")
endif()
