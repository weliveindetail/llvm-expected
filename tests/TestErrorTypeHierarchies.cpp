#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

#include "Common.h"

class IOError : public llvm::ErrorInfo<IOError> {
public:
  IOError() = default;
  virtual void log(std::ostream &OS) const {};
  virtual std::error_code convertToErrorCode() const {
    return llvm::inconvertibleErrorCode();
  };
  static char ID;
};

class FileNotFoundError : public llvm::ErrorInfo<FileNotFoundError, IOError> {
public:
  FileNotFoundError() = default;
  static char ID;
};

class AccessDeniedError : public llvm::ErrorInfo<AccessDeniedError, IOError> {
public:
  AccessDeniedError() = default;
  static char ID;
};

char IOError::ID;
char FileNotFoundError::ID;
char AccessDeniedError::ID;

TEST(ErrorHierarchies, Simple)
{
  using namespace llvm;
  std::vector<std::string> files { "x", "y", "z" };

  auto openFile = [](std::string file) {
    if (file == "x")
      return llvm::make_error<FileNotFoundError>();
    if (file == "y")
      return llvm::make_error<AccessDeniedError>();
    if (file == "z")
      return llvm::make_error<IOError>();

    return Error(Error::success());
  };

  for (auto file : files) {
    llvm::handleAllErrors(
      openFile(file),
      [file](const FileNotFoundError &err) {
        EXPECT_TRUE(file == "x");
      },
      [file](const IOError &err) {
        EXPECT_TRUE(file == "y" || file == "z");
      });
  }
}
