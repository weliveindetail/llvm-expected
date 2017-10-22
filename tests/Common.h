#pragma once

#include <Expected.h>
#include <Errors.h>

#include "ExtraPayloadError.h"

inline bool isInSuccessState(llvm::Error &err) {
  return !err;
}

template <class T>
bool isInSuccessState(llvm::Expected<T> &exp) {
  return !!exp;
}

inline llvm::Expected<int> Expected_AlwaysSuccess() {
  return 0;
}

inline llvm::Expected<int> Expected_AlwaysStringError() {
  return llvm::make_error<llvm::StringError>("StringErrorMessage",
                                             llvm::inconvertibleErrorCode());
}

inline llvm::Expected<int> Expected_AlwaysExtraPayloadError() {
  return llvm::make_error<ExtraPayloadError>(makeExtraPayload());
}

inline llvm::Expected<int> Expected_AlwaysErrorList() {
  auto first = Expected_AlwaysExtraPayloadError();
  auto second = Expected_AlwaysStringError();

  return llvm::joinErrors(first.takeError(), second.takeError());
}
