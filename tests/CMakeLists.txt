cmake_minimum_required(VERSION 3.2)

add_executable(TestLLVMExpected
    TestLLVMExpected.cpp

    TestErrorList.cpp
    TestErrorSuccess.cpp
    TestExtraPayloadError.cpp
    TestStringError.cpp
    TestForceAllErrorsDebug.cpp
    TestForceAllErrorsRelease.cpp

    TestForceAllErrorsChallenges.cpp

    Common.h
    CustomError.h
    CustomError.cpp
    ExtraPayloadError.h
    ExtraPayloadError.cpp

    ErrorTest.cpp # copied from /unittests/Support/ErrorTest.cpp
)

set_target_properties(TestLLVMExpected PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)

# add GTest libraries
find_package(GTest REQUIRED)
enable_testing()

target_include_directories(TestLLVMExpected PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TestLLVMExpected PRIVATE
    LLVMExpected
    GTest::GTest
)

message("")
message("TestLLVMExpected")

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
