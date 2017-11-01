cmake_minimum_required(VERSION 3.2)

set(BUILD_GTEST ON CACHE BOOL "Include googletest" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Don't include googlemock" FORCE)

cmake_policy(PUSH)
add_subdirectory(googletest)
cmake_policy(POP)

enable_testing()

add_executable(TestLLVMExpected
    TestLLVMExpected.cpp

    TestErrorList.cpp
    TestErrorSuccess.cpp
    TestErrorTypeHierarchies.cpp
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

target_include_directories(TestLLVMExpected PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_link_libraries(TestLLVMExpected PRIVATE
    LLVMExpected
    gtest
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
