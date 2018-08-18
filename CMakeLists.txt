cmake_minimum_required(VERSION 3.4.3)

# this is only the unit test for llvm-expected
# the library itself is header-only
project(llvm-expected-test)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  set(DISABLE_RTTI_FLAG "/GR-")
  set(DISABLE_EXCEPTIONS_FLAG "")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
  set(DISABLE_RTTI_FLAG "-fno-rtti")
  set(DISABLE_EXCEPTIONS_FLAG "-fno-exceptions")
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
message("global")

message(STATUS "System: ${CMAKE_SYSTEM_NAME}")
message(STATUS "Compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "Default C++ flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Default C++ flags Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Default C++ flags Release: ${CMAKE_CXX_FLAGS_RELEASE}")

set(BUILD_GTEST ON CACHE BOOL "Include googletest" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Don't include googlemock" FORCE)

cmake_policy(PUSH)
add_subdirectory(googletest)
cmake_policy(POP)

enable_testing()

add_executable(llvm-expected-test
  llvm-expected-test.cpp)

target_link_libraries(llvm-expected-test PRIVATE 
  gtest gtest_main)

target_include_directories(llvm-expected-test PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR})

target_compile_options(llvm-expected-test PRIVATE
  ${DISABLE_RTTI_FLAG}
  ${DISABLE_EXCEPTIONS_FLAG})

set_target_properties(llvm-expected-test PROPERTIES
  CXX_STANDARD 17)

message("")
message("llvm-expected-test")

get_target_property(Prop TestLLVMExpected INCLUDE_DIRECTORIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Include directories: ${Prop}")
endif()

get_target_property(Prop TestLLVMExpected LINK_LIBRARIES)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Link libraries: ${Prop}")
endif()

get_target_property(Prop TestLLVMExpected COMPILE_OPTIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile options: ${Prop}")
endif()

get_target_property(Prop TestLLVMExpected COMPILE_DEFINITIONS)
if (NOT Prop STREQUAL "Prop-NOTFOUND")
    message(STATUS "Compile definitions: ${Prop}")
endif()

message("")
message("global")
