#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>
#include <ForceAllErrors.h>

#include "Common.h"
#include "CustomError.h"

#ifndef NDEBUG

static int ForceAllErrors_sideEffects0_value = 0;
static int ForceAllErrors_sideEffects1_value = 0;

// -----------------------------------------------------------------------------

// Example function under test:
llvm::Expected<int> ForceAllErrors_sideEffects0(bool returnInt) {
  // Non-error code path. Error sanitizer will break the instance created in
  // the return, not capturing the below side effect for regular error cases.
  // >>> No ideas so far.
  if (returnInt)
    return 0;

  ForceAllErrors_sideEffects0_value = 1;
  return llvm::make_error<llvm::StringError>("Message",
                                             llvm::inconvertibleErrorCode());
}

// Example invocation:
TEST(ForceAllErrors, sideEffects0_RegularError)
{
  // Reset side effect value and call function that modifies it in error cases.
  // Invoke function in a way that it returns an error.
  ForceAllErrors_sideEffects0_value = 0;
  auto expected = ForceAllErrors_sideEffects0(false);

  // Instance broken via regular execution path
  EXPECT_FALSE(isInSuccessState(expected));

  // Side effect captured: consistent state
  EXPECT_EQ(1, ForceAllErrors_sideEffects0_value);

  llvm::consumeError(expected.takeError());
}

// Challenge for ESan:
TEST(ForceAllErrors, sideEffects0_ESanChallenge)
{
  int breakInstance = 1;
  llvm::ForceAllErrorsInScope FAE(breakInstance);

  // Reset side effect value and call function that modifies it in error cases.
  // Invoke function in a way it would usually return NO error
  ForceAllErrors_sideEffects0_value = 0;
  auto expected = ForceAllErrors_sideEffects0(true);

  // Instance broken with mocked error by error sanitizer
  EXPECT_FALSE(isInSuccessState(expected));

  // Side effect not captured: INCONSISTENT state
  EXPECT_EQ(0, ForceAllErrors_sideEffects0_value);

  llvm::consumeError(expected.takeError());
}

// -----------------------------------------------------------------------------

// Example function under test:
llvm::Expected<int> ForceAllErrors_sideEffects1(bool returnInt) {
  std::unique_ptr<llvm::Error> err;

  if (!returnInt)
    err = std::make_unique<llvm::Error>(
        llvm::make_error<llvm::StringError>("Message",
                                            llvm::inconvertibleErrorCode()));

  if (err) {
    ForceAllErrors_sideEffects1_value = 1;
  }

  // ... more code ...

  if (!returnInt)
    return std::move(*err);

  // Error sanitizer will break this instance, not capturing above side effect.
  // >>> Instrumentation of llvm::Error may help?!
  return 0;
}

// Example invocation:
TEST(ForceAllErrors, sideEffects1_RegularError)
{
  // Reset side effect value and call function that modifies it in error cases.
  // Invoke function in a way that it returns an error.
  ForceAllErrors_sideEffects1_value = 0;
  auto expected = ForceAllErrors_sideEffects1(false);

  // Instance broken via regular execution path
  EXPECT_FALSE(isInSuccessState(expected));

  // Side effect captured: consistent state
  EXPECT_EQ(1, ForceAllErrors_sideEffects1_value);

  llvm::consumeError(expected.takeError());
}

// Challenge for ESan:
TEST(ForceAllErrors, sideEffects1_ESanChallenge)
{
  // Setup ESan to break first instance of llvm::Expected<T>
  int breakInstance = 1;
  llvm::ForceAllErrorsInScope FAE(breakInstance);
  
  // Reset side effect value and call function that modifies it in error cases.
  // Invoke function in a way it would usually return NO error.
  ForceAllErrors_sideEffects1_value = 0;
  auto expected = ForceAllErrors_sideEffects1(true);

  // Instance broken with mocked error by error sanitizer
  EXPECT_FALSE(isInSuccessState(expected));

  // Side effect not captured: INCONSISTENT state
  EXPECT_EQ(0, ForceAllErrors_sideEffects1_value);

  llvm::consumeError(expected.takeError());
}

// -----------------------------------------------------------------------------

// Example function under test:
llvm::Expected<int> ForceAllErrors_mockedErrorType0(bool returnInt) {
  // Non-error code path. Error sanitizer will break the instance created in
  // the return. How to determine the correct error type is CustomError?
  // >>> Static analysis can do this: remember which functions emit which errors
  if (returnInt)
    return 0;

  return llvm::make_error<CustomError>(__FILE__, __LINE__);
}

// Example invocation:
TEST(ForceAllErrors, mockedErrorType0_RegularError)
{
  // Invoke function in a way that it returns an error
  auto expected = ForceAllErrors_mockedErrorType0(false);
  EXPECT_FALSE(isInSuccessState(expected));

  // The expected error type is CustomError, not llvm::StringError
  llvm::Error err = expected.takeError();
  EXPECT_TRUE(err.isA<CustomError>());

  llvm::consumeError(std::move(err));
}

// Challenge for ESan:
TEST(ForceAllErrors, mockedErrorType0_ESanChallenge)
{
  // Setup ESan to break first instance of llvm::Expected<T>
  int breakInstance = 1;
  llvm::ForceAllErrorsInScope FAE(breakInstance);

  // Invoke function in a way it would usually return NO error
  auto expected = ForceAllErrors_mockedErrorType0(true);
  EXPECT_FALSE(isInSuccessState(expected));

  // The expected error type is CustomError. However, it's a mocked error type
  // actually (currently llvm::StringError).
  llvm::Error err = expected.takeError();
  EXPECT_TRUE(err.isA<llvm::StringError>());

  llvm::consumeError(std::move(err));
}

// -----------------------------------------------------------------------------

// Example function under test:
llvm::Expected<int> ForceAllErrors_mockedErrorType1(bool returnInt, bool errTypeCustom) {
  if (!returnInt) {
    if (errTypeCustom)
      return llvm::make_error<CustomError>(__FILE__, __LINE__);
    else
      return llvm::make_error<llvm::StringError>(
          "Message", llvm::inconvertibleErrorCode());
  }

  // Non-error code path. Error sanitizer will break this instance. How to
  // determine the possible error types?
  // >>> Static analysis can help here too.
  // >>> Add feature so that error can be forced twice. Once with each type.
  return 0;
}

// Example invocation:
TEST(ForceAllErrors, mockedErrorType1_RegularError)
{
  // Invoke function in a way that it returns a CustomError
  auto expected1 = ForceAllErrors_mockedErrorType1(false, true);
  EXPECT_FALSE(isInSuccessState(expected1));

  llvm::Error err1 = expected1.takeError();
  EXPECT_TRUE(err1.isA<CustomError>());

  // Invoke function in a way that it returns a llvm::StringError
  auto expected2 = ForceAllErrors_mockedErrorType1(false, false);
  EXPECT_FALSE(isInSuccessState(expected2));

  llvm::Error err2 = expected2.takeError();
  EXPECT_TRUE(err2.isA<llvm::StringError>());

  llvm::consumeError(std::move(err1));
  llvm::consumeError(std::move(err2));
}

// Challenge for ESan:
TEST(ForceAllErrors, mockedErrorType1_ESanChallenge)
{
  // Setup ESan to break first instance of llvm::Expected<T>.
  // >>> Add feature to break one and the same instance multiple times
  // >>> mocking all possible error types.
  int breakInstance = 1;
  llvm::ForceAllErrorsInScope FAE(breakInstance);

  // Invoke function in a way it would usually return NO error
  auto expected = ForceAllErrors_mockedErrorType1(true, true);
  EXPECT_FALSE(isInSuccessState(expected));

  llvm::Error err = expected.takeError();
  EXPECT_TRUE(err.isA<llvm::StringError>());

  llvm::consumeError(std::move(err));
}

// -----------------------------------------------------------------------------

// Example function under test:
llvm::Expected<int> ForceAllErrors_mockedErrorType2(std::function<llvm::Error()> f) {
  if (f)
    return f();

  // Non-error code path. Error sanitizer will break this instance. How to
  // determine the possible error types?
  // >>> Dynamic parameter determines actual error type.
  // >>> Static analysis CANNOT help.
  // >>> Add runtime-feature to record execution path dynamically.
  // >>> Use static analysis data to determine possible error types.
  return 0;
}

// Example invocation:
TEST(ForceAllErrors, mockedErrorType2_RegularError)
{
  // Invoke function in a way that it returns a CustomError
  auto expected = ForceAllErrors_mockedErrorType2([]() {
    return llvm::make_error<CustomError>(__FILE__, __LINE__);
  });

  EXPECT_FALSE(isInSuccessState(expected));

  llvm::Error err = expected.takeError();
  EXPECT_TRUE(err.isA<CustomError>());

  llvm::consumeError(std::move(err));
}

// Challenge for ESan:
TEST(ForceAllErrors, mockedErrorType2_ESanChallenge)
{
  // Setup ESan to break first instance of llvm::Expected<T>
  int breakInstance = 1;
  llvm::ForceAllErrorsInScope FAE(breakInstance);

  // Invoke function in a way it would usually return NO error AT ALL
  auto expected = ForceAllErrors_mockedErrorType2(nullptr);

  EXPECT_FALSE(isInSuccessState(expected));

  // We get the mocked error type again (currently llvm::StringError)
  llvm::Error err = expected.takeError();
  EXPECT_TRUE(err.isA<llvm::StringError>());

  llvm::consumeError(std::move(err));
}

// -----------------------------------------------------------------------------

// Example function under test:
llvm::Expected<int> ForceAllErrors_innerLoopError(bool intOrErr) {
  if (intOrErr)
    return 0;

  return llvm::make_error<llvm::StringError>(
      "Message", llvm::inconvertibleErrorCode());
}

// Example invocation:
TEST(ForceAllErrors, innerLoopError_RegularError)
{
  for (int i = 0; i < 3; ++i) {
    // Invoke function in a way it returns an error
    auto expected = ForceAllErrors_innerLoopError(false);

    EXPECT_FALSE(isInSuccessState(expected));
    llvm::consumeError(expected.takeError());
  }
}

// Challenge for ESan:
TEST(ForceAllErrors, innerLoopError_ESanChallenge)
{
  // Setup ESan to break the respective instance of llvm::Expected<T> created
  // in the inner loop. This adds a lot of cases and has little use as it tests
  // the same error paths again and again.
  // >>> Additionally collect code locations for instances
  // >>> Avoid breaking instances at same locations over and over again
  for (int breakInstance = 1; breakInstance <= 3; ++breakInstance) {
    llvm::ForceAllErrorsInScope FAE(breakInstance);

    for (int i = 1; i <= 3; ++i) {
      // Invoke function in a way it would usually return no error
      auto expected = ForceAllErrors_innerLoopError(true);

      bool instanceBroken = (breakInstance == i);
      EXPECT_EQ(!instanceBroken, isInSuccessState(expected));

      llvm::consumeError(expected.takeError());
    }
  }
}

// -----------------------------------------------------------------------------

// Example functions under test:
llvm::Expected<int> ForceAllErrors_nestedError(bool intOrErr) {
  if (intOrErr)
    return 0;

  return llvm::make_error<llvm::StringError>(
      "Message", llvm::inconvertibleErrorCode());
}

llvm::Expected<int> ForceAllErrors_recoverFromNestedError(llvm::Error err) {
  llvm::consumeError(std::move(err));

  // Currently this instance of llvm::Expected<T> will not be recognized by
  // ESan, as it only exists in an error path.
  // >>> Add feature to rerun cases that encounter new llvm::Expected<T>
  // >>> instances after an error was forced.
  // >>> Worst case up to 2^N reruns to capture all combinatons of subsequent
  // >>> errors.
  return 0;
}

llvm::Expected<int> ForceAllErrors_cascadingError(bool intOrErr) {
  auto expected = ForceAllErrors_nestedError(intOrErr);
  if (!expected)
    return ForceAllErrors_recoverFromNestedError(expected.takeError());

  return 0;
}

// Example invocation:
TEST(ForceAllErrors, cascadingError_RegularError)
{
  // Invoke function in a way it returns no error
  auto expected1 = ForceAllErrors_cascadingError(true);

  EXPECT_TRUE(isInSuccessState(expected1));
  llvm::consumeError(expected1.takeError());

  // Invoke function in a way the error gets handled in recoverFromNestedError
  auto expected2 = ForceAllErrors_cascadingError(false);

  EXPECT_TRUE(isInSuccessState(expected2));
  llvm::consumeError(expected2.takeError());
}

// Challenge for ESan:
TEST(ForceAllErrors, cascadingError_ESanChallenge)
{
  {
    // Setup ESan to break the first instance of llvm::Expected<T>.
    // It's the one created in the nestedError function.
    int breakInstance = 1;
    llvm::ForceAllErrorsInScope FAE(breakInstance);

    // Invoke function in a way it would usually return no error
    auto expected = ForceAllErrors_cascadingError(true);

    // Handled in recoverFromNestedError
    EXPECT_TRUE(isInSuccessState(expected));
    llvm::consumeError(expected.takeError());
  }
  {
    // Setup ESan to break the second instance of llvm::Expected<T>
    int breakInstance = 2;
    llvm::ForceAllErrorsInScope FAE(breakInstance);

    // Invoke function in a way it would usually return no error
    auto expected = ForceAllErrors_cascadingError(true);

    // Second forced error is NOT the one in created at the end of the
    // recoverFromNestedError function, as it's not entered in this case.
    // Instead it's be the one returned from the cascadingError function.
    EXPECT_FALSE(isInSuccessState(expected));
    llvm::consumeError(expected.takeError());
  }
  {
    // Setup ESan to break the third instance of llvm::Expected<T>.
    // However, there won't be a third instance, as again the
    // recoverFromNestedError function is not entered.
    int breakInstance = 3;
    llvm::ForceAllErrorsInScope FAE(breakInstance);

    // Invoke function in a way it would usually return no error
    auto expected = ForceAllErrors_cascadingError(true);

    // No forced error
    EXPECT_TRUE(isInSuccessState(expected));
    llvm::consumeError(expected.takeError());
  }
}

#endif
