cmake_minimum_required(VERSION 3.2)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(ENABLE_RTTI_FLAG /GR)
    set(ENABLE_EXCEPTIONS_FLAG /EHsc)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(ENABLE_RTTI_FLAG -frtti)
    set(ENABLE_EXCEPTIONS_FLAG -fexceptions)
else()
    message(FATAL_ERROR "Compiler not supported")
endif()

add_executable(BenchmarkException
    BenchmarkException.cpp

    BenchOverheadExample.cpp
    BenchSuccessRates.cpp

    FastRand.h
)

set_target_properties(BenchmarkException PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# add Google Benchmark libraries
find_package(benchmark REQUIRED)

target_include_directories(BenchmarkException PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(BenchmarkException PUBLIC 
    ${ENABLE_RTTI_FLAG}
    ${ENABLE_EXCEPTIONS_FLAG}
    ${DISABLE_STACKPROTECTOR_FLAG}
    ${DISABLE_FRAMEPOINTER_FLAG}
)

target_link_libraries(BenchmarkException PRIVATE
    benchmark::benchmark
)

message("")
message("BenchmarkException")

get_target_property(Prop BenchmarkException INCLUDE_DIRECTORIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Include directories: ${Prop}")
endif()

get_target_property(Prop BenchmarkException LINK_LIBRARIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Link libraries: ${Prop}")
endif()

get_target_property(Prop BenchmarkException COMPILE_OPTIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile options: ${Prop}")
endif()

get_target_property(Prop BenchmarkException COMPILE_DEFINITIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile definitions: ${Prop}")
endif()
