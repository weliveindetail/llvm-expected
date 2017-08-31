#pragma once

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

#include "Common.h"

llvm::Expected<int> Expected_AlwaysSuccess() {
  return 0;
}

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
