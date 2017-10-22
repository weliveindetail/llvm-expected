#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>
#include <ForceAllErrors.h>

#include "Common.h"

#ifdef NDEBUG

llvm::Expected<int> Expected_BreakThisInstance() {
  return 0;
}

TEST(ForceAllErrors, countInstances_NotInRelease)
{
  int countInstances = 0;
  llvm::ForceAllErrorsInScope FAE(countInstances); // no effect in Release builds
  EXPECT_TRUE(FAE.isModeCounting());

  auto expected = Expected_BreakThisInstance();
  EXPECT_TRUE(isInSuccessState(expected));
  EXPECT_EQ(0, FAE.getNumMutationPoints());

  llvm::consumeError(expected.takeError());
}

TEST(ForceAllErrors, breakInstance1_NotInRelease)
{
  int breakInstance = 1;
  llvm::ForceAllErrorsInScope FAE(breakInstance); // no effect in Release builds

  auto expected = Expected_BreakThisInstance();
  EXPECT_TRUE(isInSuccessState(expected));

  llvm::handleAllErrors(expected.takeError(),
    [](const llvm::StringError &err) {
      EXPECT_TRUE(false); // unreachable
    },
    [](const llvm::ErrorInfoBase &err) {
      EXPECT_TRUE(false); // unreachable
    }
  );
}

#endif
