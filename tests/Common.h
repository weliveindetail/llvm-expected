#pragma once

#include <Expected.h>
#include <Errors.h>

llvm::ExitOnError ExitOnErr;

void registerExitOnErr() {
  ExitOnErr.setBanner("Exit with error: ");
  ExitOnErr.setExitCodeMapper(
    [](const llvm::Error &err) {
      if (err.isA<llvm::StringError>())
        return 2;

      return 1;
    }
  );
}

bool isInSuccessState(llvm::Error &err) {
  return !err;
}

template <class T>
bool isInSuccessState(llvm::Expected<T> &exp) {
  return !!exp;
}
