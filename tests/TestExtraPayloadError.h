#pragma once

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

#include "Common.h"
#include "ExtraPayloadError.h"

llvm::Expected<int> Expected_AlwaysExtraPayloadError() {
  return llvm::make_error<ExtraPayloadError>(makeExtraPayload());
}

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
