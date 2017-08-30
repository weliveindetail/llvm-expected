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

    include/ErrorBase.h
    include/Errors.h
    include/Expected.h

    lib/ErrorBase.cpp
)

set_target_properties(TestLLVMExpected PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# add GTest libraries
find_package(GTest REQUIRED)
enable_testing()

target_compile_options(TestLLVMExpected PRIVATE ${DISABLE_RTTI_FLAG})

target_include_directories(TestLLVMExpected PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries(TestLLVMExpected PRIVATE
    GTest::GTest
    GTest::Main
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
