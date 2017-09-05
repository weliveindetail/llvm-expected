#include "CustomError.h"
#include <Errors.h>

char CustomError::ID = 0;

CustomError::CustomError(std::string file, int line)
    : File(std::move(file)), Line(std::move(line)) {}

void CustomError::log(std::ostream &OS) const {
  OS << "CustomError in file " << File << ":" << Line << ": something wrong\n";
}

std::error_code CustomError::convertToErrorCode() const {
  return llvm::inconvertibleErrorCode();
}
