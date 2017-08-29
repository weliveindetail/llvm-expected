cmake_minimum_required(VERSION 3.2)

project(TestLLVMExpected VERSION 1.0 LANGUAGES CXX)

if(WIN32)
    set(OS_NAME "Win")
    set(DISABLE_RTTI_FLAG /GR-)
elseif(APPLE)
    set(OS_NAME "OSX")
    set(DISABLE_RTTI_FLAG -fno-rtti)
elseif(UNIX)
    set(OS_NAME "Linux")
    set(DISABLE_RTTI_FLAG -fno-rtti)
else()
    message(FATAL_ERROR "Operating system not supported")
endif()
message(STATUS "System: ${OS_NAME}")

add_executable(TestLLVMExpected
    TestLLVMExpected.cpp

    ExtraPayloadError.h
    ExtraPayloadError.cpp

    include/llvm/Support/AlignOf.h
    include/llvm/Support/Compiler.h
    include/llvm/Support/Error.h
    include/llvm/Support/ErrorHandling.h

    lib/Support/Error.cpp
    lib/Support/ErrorHandling.cpp
)

set_target_properties(TestLLVMExpected PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# add GTest libraries
find_package(GTest REQUIRED)
enable_testing()

# add LLVM libraries, set LLVM_* variables
find_package(LLVM 5.0 REQUIRED PATHS ${LLVM_DIR})
message(STATUS "LLVM_DIR: ${LLVM_DIR}")
message(STATUS "LLVM_PACKAGE_VERSION: ${LLVM_PACKAGE_VERSION}")

if(NOT LLVM_ENABLE_RTTI)
    target_compile_options(TestLLVMExpected PRIVATE ${DISABLE_RTTI_FLAG})
endif()

message(STATUS "LLVM Source Directory: ${LLVM_BUILD_MAIN_SRC_DIR}")
message(STATUS "LLVM Build Directory: ${LLVM_BUILD_BINARY_DIR}")

target_include_directories(TestLLVMExpected PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${LLVM_INCLUDE_DIRS}
)

# LLVM definitions
separate_arguments(LLVM_DEFINITIONS)
target_compile_definitions(TestLLVMExpected PRIVATE
    ${LLVM_DEFINITIONS}
)

# LLVM dependencies
llvm_map_components_to_libnames(LLVM_LIBS
    core
    support
)

target_link_libraries(TestLLVMExpected PRIVATE
    GTest::GTest
    GTest::Main
    ${LLVM_LIBS}
)

message("")
message("TestLLVMExpected")

get_target_property(ALL_ID TestLLVMExpected INCLUDE_DIRECTORIES)
message(STATUS "Include directories: ${ALL_ID}")

get_target_property(ALL_LL TestLLVMExpected LINK_LIBRARIES)
message(STATUS "Link libraries: ${ALL_LL}")

get_target_property(ALL_CO TestLLVMExpected COMPILE_OPTIONS)
message(STATUS "Compile options: ${ALL_CO}")

get_target_property(ALL_CD TestLLVMExpected COMPILE_DEFINITIONS)
message(STATUS "Compile definitions: ${ALL_CD}")

message(STATUS "Other flags: ${CMAKE_CXX_FLAGS}")
message(STATUS "Other flags Debug: ${CMAKE_CXX_FLAGS_DEBUG}")
message(STATUS "Other flags Release: ${CMAKE_CXX_FLAGS_RELEASE}")
