#include <gtest/gtest.h>

#include "Common.h"
#include "TestErrorSuccess.h"
#include "TestStringError.h"
#include "TestErrorList.h"
#include "TestExtraPayloadError.h"

#ifdef NDEBUG
#include "TestForceAllErrorsRelease.h"
#else
#include "TestForceAllErrorsDebug.h"
#endif

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);

  registerExitOnErr();

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
