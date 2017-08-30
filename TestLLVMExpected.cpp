#include <memory>
#include <string>

#include <gtest/gtest.h>

#include "llvm/Support/Error.h"

#include "ExtraPayloadError.h"

// ----------------------------------------------------------------------------

llvm::ExitOnError ExitOnErr;

llvm::Expected<int> Expected_AlwaysSuccess() {
  return 0;
}

llvm::Expected<int> Expected_AlwaysStringError() {
  return llvm::make_error<llvm::StringError>("StringErrorMessage",
                                             llvm::inconvertibleErrorCode());
}

llvm::Expected<int> Expected_AlwaysExtraPayloadError() {
  return llvm::make_error<ExtraPayloadError>(makeExtraPayload());
}

llvm::Expected<int> Expected_AlwaysErrorList() {
  auto first = Expected_AlwaysExtraPayloadError();
  auto second = Expected_AlwaysStringError();

  return llvm::joinErrors(first.takeError(), second.takeError());
}

bool isInSuccessState(llvm::Error &err) {
  return !err;
}

template <class T>
bool isInSuccessState(llvm::Expected<T> &exp) {
  return !!exp;
}

// ----------------------------------------------------------------------------

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
  ExitOnErr(Expected_AlwaysSuccess()); // no death
}

// ----------------------------------------------------------------------------

TEST(StringError, ExitOnError)
{
  EXPECT_EXIT(ExitOnErr(Expected_AlwaysStringError()),
              ::testing::ExitedWithCode(2), "");
}

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

// ----------------------------------------------------------------------------

TEST(ExtraPayloadError, handleAllErrors)
{
  auto expected = Expected_AlwaysExtraPayloadError();
  EXPECT_FALSE(isInSuccessState(expected));

  llvm::handleAllErrors(expected.takeError(),
    [](ExtraPayloadError &err) {
      consumeExtraPayload(err.takeExtraPayload());
    },
    [](const llvm::ErrorInfoBase &err) {
      EXPECT_TRUE(false); // unreachable
    }
  );
}

// ----------------------------------------------------------------------------

TEST(ErrorList, handleAllErrors)
{
  auto expected = Expected_AlwaysErrorList();
  EXPECT_FALSE(isInSuccessState(expected));

  int order = 0;
  llvm::handleAllErrors(expected.takeError(),
    [&](const llvm::StringError &err) {
      EXPECT_EQ(1, order); // handled second
      ++order;
    },
    [&](ExtraPayloadError &err) {
      consumeExtraPayload(err.takeExtraPayload());

      EXPECT_EQ(0, order); // handled first
      ++order;
    },
    [](const llvm::ErrorInfoBase &err) {
      EXPECT_TRUE(false);  // unreachable
    }
  );
}

// ----------------------------------------------------------------------------

int main(int argc, char **argv) {
  ExitOnErr.setBanner("Exit with error: ");
  ExitOnErr.setExitCodeMapper(
    [](const llvm::Error &err) {
      if (err.isA<llvm::StringError>())
        return 2;

      return 1;
    }
  );

  ::testing::InitGoogleTest(&argc, argv);
  ::testing::GTEST_FLAG(catch_exceptions) = false;
  ::testing::GTEST_FLAG(throw_on_failure) = true;

  int exitCode = RUN_ALL_TESTS();

  // on windows don't close the console window immediately
  #ifdef _WIN32
    fflush(stdout);
    getchar();
  #endif

  return exitCode;
}
