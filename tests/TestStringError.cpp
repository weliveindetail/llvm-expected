#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

#include "Common.h"

TEST(StringError, consumeError)
{
  auto expected = Expected_AlwaysStringError();
  EXPECT_FALSE(isInSuccessState(expected));

  llvm::consumeError(expected.takeError());
}

TEST(StringError, handleErrors)
{
  auto expected = Expected_AlwaysStringError();
  EXPECT_FALSE(isInSuccessState(expected));

  llvm::Error remnant = llvm::handleErrors(expected.takeError(),
    [](const llvm::StringError &err) {
      EXPECT_EQ("StringErrorMessage", err.getMessage());
      EXPECT_EQ(llvm::inconvertibleErrorCode(), err.convertToErrorCode());
    }
  );

  EXPECT_TRUE(isInSuccessState(remnant));
}

TEST(StringError, handleAllErrors)
{
  auto expected = Expected_AlwaysStringError();
  EXPECT_FALSE(isInSuccessState(expected));

  llvm::handleAllErrors(expected.takeError(),
    [](const llvm::StringError &err) {
      EXPECT_EQ("StringErrorMessage", err.getMessage());
      EXPECT_EQ(llvm::inconvertibleErrorCode(), err.convertToErrorCode());
    },
    [](const llvm::ErrorInfoBase &err) {
      EXPECT_TRUE(false); // unreachable
    }
  );
}

// see main() in TestLLVMExpected.cpp
TEST(StringError, ExitOnError)
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

  EXPECT_EXIT(ExitOnErr(Expected_AlwaysStringError()),
              ::testing::ExitedWithCode(2),
              "Exit with error: StringErrorMessage");
}
