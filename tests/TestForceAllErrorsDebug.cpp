#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>
#include <ForceAllErrors.h>

#include "Common.h"

#ifndef NDEBUG

llvm::Expected<int> Expected_CountThisInstance() {
  return 0;
}

llvm::Expected<int> Expected_BreakThisInstance() {
  return 0;
}

llvm::Expected<int> Expected_BreakNextInstance() {
  return 0;
}

TEST(ForceAllErrors, countInstances1)
{
  int countInstances = 0;
  llvm::ForceAllErrorsInScope FAE(countInstances);
  EXPECT_TRUE(FAE.isModeCounting());

  auto expected = Expected_CountThisInstance();
  EXPECT_TRUE(isInSuccessState(expected));
  EXPECT_EQ(1, FAE.getNumMutationPoints());

  llvm::consumeError(expected.takeError());
}

TEST(ForceAllErrors, countInstances2)
{
  int countInstances = 0;
  llvm::ForceAllErrorsInScope FAE(countInstances);
  EXPECT_TRUE(FAE.isModeCounting());

  auto expected1 = Expected_CountThisInstance();
  EXPECT_TRUE(isInSuccessState(expected1));
  llvm::consumeError(expected1.takeError());

  auto expected2 = Expected_CountThisInstance();
  EXPECT_TRUE(isInSuccessState(expected2));
  llvm::consumeError(expected2.takeError());

  EXPECT_EQ(2, FAE.getNumMutationPoints());
}

TEST(ForceAllErrors, countInstances3)
{
  int countInstances = 0;
  llvm::ForceAllErrorsInScope FAE(countInstances);
  EXPECT_TRUE(FAE.isModeCounting());

  for (int i = 0; i < 3; i++) {
    auto expected = Expected_CountThisInstance();
    EXPECT_TRUE(isInSuccessState(expected));
    llvm::consumeError(expected.takeError());
  }

  EXPECT_EQ(3, FAE.getNumMutationPoints());
}

TEST(ForceAllErrors, breakInstance1)
{
  int breakInstance = 1;
  llvm::ForceAllErrorsInScope FAE(breakInstance);

  auto expected = Expected_BreakThisInstance();
  EXPECT_FALSE(isInSuccessState(expected));

  llvm::handleAllErrors(expected.takeError(),
    [](const llvm::StringError &err) { // check mocked error type
      EXPECT_EQ("Mocked Error", err.getMessage());
      EXPECT_EQ(llvm::inconvertibleErrorCode(), err.convertToErrorCode());
    },
    [](const llvm::ErrorInfoBase &err) {
      EXPECT_TRUE(false); // unreachable
    }
  );
}

TEST(ForceAllErrors, breakInstance2)
{
  int breakInstance = 2;
  llvm::ForceAllErrorsInScope FAE(breakInstance);

  auto expected1 = Expected_BreakNextInstance();
  EXPECT_TRUE(isInSuccessState(expected1));
  llvm::consumeError(expected1.takeError());

  auto expected2 = Expected_BreakThisInstance();
  EXPECT_FALSE(isInSuccessState(expected2));
  llvm::consumeError(expected2.takeError());
}

#endif
