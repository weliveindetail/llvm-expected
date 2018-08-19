#include <iostream>
#include <sstream>

#include "llvm-expected.h"

llvm::Expected<int> to_int(const std::string &S, bool MatchWord = false) {
  std::istringstream ss(S);
  ss.imbue(std::locale::classic());

  int result;
  ss >> result;

  if (LLVM_UNLIKELY(ss.fail()))
    return llvm::createStringError("Failed to parse int: " + S);

  if (LLVM_UNLIKELY(MatchWord && !ss.eof()))
    return llvm::createStringError("Failed to parse full string as int: " + S);

  return result;
}

int main(int argc, char *argv[]) {
  llvm::Error Errs = llvm::Error::success();
  auto QueueError = [&Errs](llvm::Error Err) {
    Errs = llvm::joinErrors(std::move(Errs), std::move(Err));
  };

  std::cout << "Ints: ";
  static constexpr bool MatchWord = true;

  for (int i = 1; i < argc; i++) {
    if (auto parsed_int = to_int(argv[i], MatchWord)) {
      std::cout << *parsed_int << " ";
    } else {
      QueueError(parsed_int.takeError());
    }
  }

  std::cout << "\n";
  llvm::logAllUnhandledErrors(std::move(Errs), std::cout, "\nErrors:\n");
  std::cout << "\n";
  return 0;
}
