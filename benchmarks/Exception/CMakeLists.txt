cmake_minimum_required(VERSION 3.2)

add_executable(BenchmarkException
    BenchmarkException.cpp

    FwdException.cpp
    FwdIntOutcome.cpp
    FwdVoidOutcome.cpp

    PayloadException.cpp
    PayloadOutcome.cpp

    SuccessRateException.cpp
    SuccessRateOutcome.cpp

    ../common/Compiler.h
    ../common/FastRand.h
)

target_include_directories(BenchmarkException PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../outcome/include
)

# This target doesn't link the LLVMExpected library 
# and so it won't inherit any compiler options.
target_compile_options(BenchmarkException PRIVATE 
    ${USE_CXX_STANDARD_14}
    ${ENABLE_RTTI_FLAG}
    ${ENABLE_EXCEPTIONS_FLAG}
    ${DISABLE_STACKPROTECTOR_FLAG}
    ${DISABLE_FRAMEPOINTER_FLAG}
)

target_link_libraries(BenchmarkException PRIVATE
    benchmark
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
