cmake_minimum_required(VERSION 3.2)

project(LLVMExpected VERSION 1.0 LANGUAGES CXX)

option(LLVMEXPECTED_INCLUDE_TESTS      "Configure build to include the tests subproject"           OFF)
option(LLVMEXPECTED_INCLUDE_BENCHMARKS "Configure build to include the subprojects for benchmarks" OFF)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(USE_CXX_STANDARD_14 "/std:c++14")
    set(ENABLE_RTTI_FLAG "/GR")
    set(ENABLE_EXCEPTIONS_FLAG "/EHsc")
    set(DISABLE_RTTI_FLAG "/GR-")
    set(DISABLE_EXCEPTIONS_FLAG "")
    set(DISABLE_STACKPROTECTOR_FLAG "/GS-")
    set(DISABLE_FRAMEPOINTER_FLAG "/Oy")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(USE_CXX_STANDARD_14 "-std=c++14")
    set(ENABLE_RTTI_FLAG "-frtti")
    set(ENABLE_EXCEPTIONS_FLAG "-fexceptions")
    set(DISABLE_RTTI_FLAG "-fno-rtti")
    set(DISABLE_EXCEPTIONS_FLAG "-fno-exceptions")
    set(DISABLE_STACKPROTECTOR_FLAG "-fno-stack-protector")
    set(DISABLE_FRAMEPOINTER_FLAG "-fomit-frame-pointer")
else()
    message(FATAL_ERROR "Compiler not supported")
endif()

# use static runtime libraries for MSVC builds
if(MSVC)
  set(variables 
    CMAKE_CXX_FLAGS_DEBUG
    CMAKE_CXX_FLAGS_RELEASE
  )
  foreach(variable ${variables})
	if(${variable} MATCHES "/MD")
	  string(REGEX REPLACE "/MD" "/MT" ${variable} "${${variable}}")
	endif()
  endforeach()
endif()

message("")
message("Global")

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "C++ flags Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "C++ flags Release: ${CMAKE_CXX_FLAGS_RELEASE}")

add_library(LLVMExpected
    include/ErrorBase.h
    include/Errors.h
    include/Expected.h
    include/ForceAllErrors.h

    lib/ErrorBase.cpp
    lib/ForceAllErrors.cpp
)

target_include_directories(LLVMExpected PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_compile_options(LLVMExpected
  PUBLIC
    ${USE_CXX_STANDARD_14}

  PRIVATE
    ${DISABLE_RTTI_FLAG}
    ${DISABLE_EXCEPTIONS_FLAG}
    ${DISABLE_STACKPROTECTOR_FLAG}
    ${DISABLE_FRAMEPOINTER_FLAG}
)

message("")
message("LLVMExpected")

get_target_property(Prop LLVMExpected INCLUDE_DIRECTORIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Include directories: ${Prop}")
endif()

get_target_property(Prop LLVMExpected LINK_LIBRARIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Link libraries: ${Prop}")
endif()

get_target_property(Prop LLVMExpected COMPILE_OPTIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile options: ${Prop}")
endif()

get_target_property(Prop LLVMExpected COMPILE_DEFINITIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile definitions: ${Prop}")
endif()

if (${LLVMEXPECTED_INCLUDE_TESTS})
  add_subdirectory(tests)
else()
  message("")
  message("TestLLVMExpected")
  message(STATUS "Disabled")
  message(STATUS "Configure with -DLLVMEXPECTED_INCLUDE_TESTS=ON to enable")
endif()

if (${LLVMEXPECTED_INCLUDE_BENCHMARKS})
  add_subdirectory(benchmarks)
else()
  message("")
  message("BenchmarkException")
  message(STATUS "Disabled")
  message(STATUS "Configure with -DLLVMEXPECTED_INCLUDE_BENCHMARKS=ON to enable")
  message("")
  message("BenchmarkLLVMExpected")
  message(STATUS "Disabled")
  message(STATUS "Configure with -DLLVMEXPECTED_INCLUDE_BENCHMARKS=ON to enable")
endif()

message("")
message("Global")
