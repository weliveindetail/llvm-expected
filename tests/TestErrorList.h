#pragma once

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

#include "Common.h"
#include "TestStringError.h"
#include "TestExtraPayloadError.h"

llvm::Expected<int> Expected_AlwaysErrorList() {
  auto first = Expected_AlwaysExtraPayloadError();
  auto second = Expected_AlwaysStringError();

  return llvm::joinErrors(first.takeError(), second.takeError());
}

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
