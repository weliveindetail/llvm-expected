#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

#include "Common.h"

TEST(ErrorSuccessHandling, cantFail)
{
  int result = llvm::cantFail(Expected_AlwaysSuccess());
  EXPECT_EQ(0, result);
}

TEST(ErrorSuccessHandling, consumeError)
{
  auto expected = Expected_AlwaysSuccess();
  EXPECT_TRUE(isInSuccessState(expected));

  llvm::consumeError(expected.takeError());
}

TEST(ErrorSuccessHandling, ExitOnError)
{
  llvm::ExitOnError ExitOnErr;

  ExitOnErr.setBanner("Exit with error: ");
  ExitOnErr.setExitCodeMapper(
    [](const llvm::Error &err) {
      if (err.isA<llvm::StringError>())
        return 2;

      return 1;
    }
  );

  ExitOnErr(Expected_AlwaysSuccess()); // no death
}
