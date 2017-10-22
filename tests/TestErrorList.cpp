#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

#include "Common.h"

TEST(ErrorList, handleAllErrors_ListOrder)
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

TEST(ErrorList, handleAllErrors_MultiLevelOrder)
{
  auto expectedA = Expected_AlwaysExtraPayloadError();
  auto expectedB = Expected_AlwaysStringError();

  auto errorList2 = llvm::joinErrors(expectedA.takeError(),
                                     expectedB.takeError());

  auto EC = std::make_error_code(std::errc::invalid_argument);
  auto errorList3 = llvm::joinErrors(std::move(errorList2),
                                     llvm::errorCodeToError(EC));

  auto errorList4 = llvm::joinErrors(llvm::Error::success(),
                                     std::move(errorList3));

  // errorList4:
  //                ErrorList
  //               /         \
  //      ErrorSuccess     ErrorList
  //                      /         \
  //                ErrorList      ECError
  //               /         \
  // ExtraPayloadError     StringError

  int order = 0;
  llvm::handleAllErrors(std::move(errorList4),
    [&](const llvm::ErrorList &err) {
      EXPECT_TRUE(false);  // unreachable
    },
    [&](ExtraPayloadError &err) {
      consumeExtraPayload(err.takeExtraPayload());

      EXPECT_EQ(0, order); // first (Success won't call back)
      ++order;
    },
    [&](const llvm::StringError &err) {
      EXPECT_EQ(1, order); // second
      ++order;
    },
    [&](const llvm::ECError &err) {
      EXPECT_EQ(2, order); // last
      ++order;
    },
    [](const llvm::ErrorInfoBase &err) {
      EXPECT_TRUE(false);  // unreachable
    }
  );
}
