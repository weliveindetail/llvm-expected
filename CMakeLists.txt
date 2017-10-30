cmake_minimum_required(VERSION 3.2)

project(LLVMExpected VERSION 1.0 LANGUAGES CXX)

if(WIN32)
    set(OS_NAME "Win")
elseif(APPLE)
    set(OS_NAME "OSX")
elseif(UNIX)
    set(OS_NAME "Linux")
else()
    message(FATAL_ERROR "Operating system not supported")
endif()

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(DISABLE_RTTI_FLAG /GR-)
    set(DISABLE_EXCEPTIONS_FLAG "")
    set(DISABLE_STACKPROTECTOR_FLAG /GS-)
    set(DISABLE_FRAMEPOINTER_FLAG /Oy)
elseif(CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(DISABLE_RTTI_FLAG -fno-rtti)
    set(DISABLE_EXCEPTIONS_FLAG -fno-exceptions)
    set(DISABLE_STACKPROTECTOR_FLAG -fno-stack-protector)
    set(DISABLE_FRAMEPOINTER_FLAG -fomit-frame-pointer)
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

message(STATUS "System: ${OS_NAME}")
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

set_target_properties(LLVMExpected PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

target_compile_options(LLVMExpected PUBLIC 
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

add_subdirectory(tests)
add_subdirectory(benchmarks/LLVMExpected)
add_subdirectory(benchmarks/Exception)
