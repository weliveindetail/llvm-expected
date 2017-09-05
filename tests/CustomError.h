#pragma once

#include <string>
#include <ostream>
#include <system_error>

#include <ErrorBase.h>

class CustomError : public llvm::ErrorInfo<CustomError> {
public:
  static char ID;

  CustomError(std::string file, int line);

  void log(std::ostream &OS) const override;
  std::error_code convertToErrorCode() const override;

private:
  std::string File;
  int Line;
};
