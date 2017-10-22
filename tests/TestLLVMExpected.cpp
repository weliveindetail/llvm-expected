#include <gtest/gtest.h>

#include "Common.h"

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  #ifndef _WIN32
    ::testing::GTEST_FLAG(catch_exceptions) = false;
    ::testing::GTEST_FLAG(throw_on_failure) = true;
  #endif

  int exitCode = RUN_ALL_TESTS();

  // on windows don't close the console window immediately
  #ifdef _WIN32
    fflush(stdout);
    getchar();
  #endif

  return exitCode;
}
