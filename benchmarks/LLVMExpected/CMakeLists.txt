cmake_minimum_required(VERSION 3.2)

add_executable(BenchmarkLLVMExpected
    BenchmarkLLVMExpected.cpp

    BenchSuccessRates.cpp
    BenchOverheadExample.cpp
    BenchOptimizations.cpp
    BenchPerfCategories.cpp

    forwarding/FwdIntAdHoc.cpp
    forwarding/FwdIntErrorCode.cpp
    forwarding/FwdIntExpected.cpp
    forwarding/FwdIntOutcome.cpp

    forwarding/FwdVoidAdHoc.cpp
    forwarding/FwdVoidError.cpp
    forwarding/FwdVoidErrorCode.cpp
    forwarding/FwdVoidOutcome.cpp

    FastRand.h

    ../outcome/include/outcome.hpp
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

target_include_directories(BenchmarkLLVMExpected PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/..
    ${CMAKE_CURRENT_SOURCE_DIR}/../outcome/include
)

target_compile_options(BenchmarkException PRIVATE
    ${DISABLE_RTTI_FLAG}
    ${DISABLE_EXCEPTIONS_FLAG}
    ${DISABLE_STACKPROTECTOR_FLAG}
    ${DISABLE_FRAMEPOINTER_FLAG}
)

target_link_libraries(BenchmarkLLVMExpected PRIVATE
    LLVMExpected
    benchmark
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
