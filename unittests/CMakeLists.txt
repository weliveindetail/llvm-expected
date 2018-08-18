set(BUILD_GTEST ON CACHE BOOL "Include googletest" FORCE)
set(BUILD_GMOCK OFF CACHE BOOL "Don't include googlemock" FORCE)

cmake_policy(PUSH)
add_subdirectory(googletest)
cmake_policy(POP)

enable_testing()

add_executable(ErrorTest Support/ErrorTest.cpp)
target_link_libraries(ErrorTest PRIVATE gtest gtest_main)
set_target_properties(ErrorTest PROPERTIES CXX_STANDARD 17)
