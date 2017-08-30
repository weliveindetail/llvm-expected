//===- llvm/Support/Errors.h - Recoverable error handling -------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an API used to report recoverable errors.
//
//===----------------------------------------------------------------------===//

#pragma once

#include <memory>
#include <ostream>
#include <system_error>
#include <type_traits>
#include <vector>

#include "ErrorHandling.h"

namespace llvm {

/// This class wraps a std::error_code in a Error.
///
/// This is useful if you're writing an interface that returns a Error
/// (or Expected) and you want to call code that still returns
/// std::error_codes.
class ECError : public ErrorInfo<ECError> {
  friend Error errorCodeToError(std::error_code);

public:
  void setErrorCode(std::error_code EC) { this->EC = EC; }
  std::error_code convertToErrorCode() const override { return EC; }
  void log(std::ostream &OS) const override { OS << EC.message(); }

  // Used by ErrorInfo::classID.
  static char ID;

protected:
  ECError() = default;
  ECError(std::error_code EC) : EC(EC) {}

  std::error_code EC;
};

/// This class wraps a string in an Error.
///
/// StringError is useful in cases where the client is not expected to be able
/// to consume the specific error message programmatically (for example, if the
/// error message is to be presented to the user).
class StringError : public ErrorInfo<StringError> {
public:
  static char ID;

  StringError(std::string Msg, std::error_code EC);

  void log(std::ostream &OS) const override;
  std::error_code convertToErrorCode() const override;

  const std::string &getMessage() const { return Msg; }

private:
  std::string Msg;
  std::error_code EC;
};

/// Special ErrorInfo subclass representing a list of ErrorInfos.
/// Instances of this class are constructed by joinError.
class ErrorList final : public ErrorInfo<ErrorList> {
  // handleErrors needs to be able to iterate the payload list of an
  // ErrorList.
  template <typename... HandlerTs>
  friend Error handleErrors(Error E, HandlerTs &&... Handlers);

  // joinErrors is implemented in terms of join.
  friend Error joinErrors(Error, Error);

public:
  void log(std::ostream &OS) const override {
    OS << "Multiple errors:\n";
    for (auto &ErrPayload : Payloads) {
      ErrPayload->log(OS);
      OS << "\n";
    }
  }

  std::error_code convertToErrorCode() const override;

  // Used by ErrorInfo::classID.
  static char ID;

private:
  ErrorList(std::unique_ptr<ErrorInfoBase> Payload1,
            std::unique_ptr<ErrorInfoBase> Payload2) {
    assert(!Payload1->isA<ErrorList>() && !Payload2->isA<ErrorList>() &&
           "ErrorList constructor payloads should be singleton errors");
    Payloads.push_back(std::move(Payload1));
    Payloads.push_back(std::move(Payload2));
  }

  static Error join(Error E1, Error E2) {
    if (!E1)
      return E2;
    if (!E2)
      return E1;
    if (E1.isA<ErrorList>()) {
      auto &E1List = static_cast<ErrorList &>(*E1.getPtr());
      if (E2.isA<ErrorList>()) {
        auto E2Payload = E2.takePayload();
        auto &E2List = static_cast<ErrorList &>(*E2Payload);
        for (auto &Payload : E2List.Payloads)
          E1List.Payloads.push_back(std::move(Payload));
      } else
        E1List.Payloads.push_back(E2.takePayload());

      return E1;
    }
    if (E2.isA<ErrorList>()) {
      auto &E2List = static_cast<ErrorList &>(*E2.getPtr());
      E2List.Payloads.insert(E2List.Payloads.begin(), E1.takePayload());
      return E2;
    }
    return Error(std::unique_ptr<ErrorList>(
        new ErrorList(E1.takePayload(), E2.takePayload())));
  }

  std::vector<std::unique_ptr<ErrorInfoBase>> Payloads;
};

/// The value returned by this function can be returned from convertToErrorCode
/// for Error values where no sensible translation to std::error_code exists.
/// It should only be used in this situation, and should never be used where a
/// sensible conversion to std::error_code is available, as attempts to convert
/// to/from this error will result in a fatal error. (i.e. it is a programmatic
/// error to try to convert such a value).
std::error_code inconvertibleErrorCode();

/// Helper for converting an std::error_code to a Error.
Error errorCodeToError(std::error_code EC);

/// Helper for converting an ECError to a std::error_code.
///
/// This method requires that Err be Error() or an ECError, otherwise it
/// will trigger a call to abort().
std::error_code errorToErrorCode(Error Err);

/// Concatenate errors. The resulting Error is unchecked, and contains the
/// ErrorInfo(s), if any, contained in E1, followed by the
/// ErrorInfo(s), if any, contained in E2.
inline Error joinErrors(Error E1, Error E2) {
  return ErrorList::join(std::move(E1), std::move(E2));
}

/// Pass the ErrorInfo(s) contained in E to their respective handlers. Any
/// unhandled errors (or Errors returned by handlers) are re-concatenated and
/// returned.
/// Because this function returns an error, its result must also be checked
/// or returned. If you intend to handle all errors use handleAllErrors
/// (which returns void, and will abort() on unhandled errors) instead.
template <typename... HandlerTs>
Error handleErrors(Error E, HandlerTs &&... Hs) {
  if (!E)
    return Error::success();

  std::unique_ptr<ErrorInfoBase> Payload = E.takePayload();

  if (Payload->isA<ErrorList>()) {
    ErrorList &List = static_cast<ErrorList &>(*Payload);
    Error R;
    for (auto &P : List.Payloads)
      R = ErrorList::join(
          std::move(R),
          handleErrorImpl(std::move(P), std::forward<HandlerTs>(Hs)...));
    return R;
  }

  return handleErrorImpl(std::move(Payload), std::forward<HandlerTs>(Hs)...);
}

/// Behaves the same as handleErrors, except that it requires that all
/// errors be handled by the given handlers. If any unhandled error remains
/// after the handlers have run, abort() will be called.
template <typename... HandlerTs>
void handleAllErrors(Error E, HandlerTs &&... Handlers) {
  auto F = handleErrors(std::move(E), std::forward<HandlerTs>(Handlers)...);
  // Cast 'F' to bool to set the 'Checked' flag if it's a success value:
  (void)!F;
}

/// Check that E is a non-error, then drop it.
inline void handleAllErrors(Error E) {
  // Cast 'E' to a bool to set the 'Checked' flag if it's a success value:
  (void)!E;
}

/// Log all errors (if any) in E to OS. If there are any errors, ErrorBanner
/// will be printed before the first one is logged. A newline will be printed
/// after each error.
///
/// This is useful in the base level of your program to allow clean termination
/// (allowing clean deallocation of resources, etc.), while reporting error
/// information to the user.
void logAllUnhandledErrors(Error E, std::ostream &OS, std::string ErrorBanner);

/// Write all error messages (if any) in E to a string. The newline character
/// is used to separate error messages.
inline std::string toString(Error E) {
  std::vector<std::string> Errors;
  handleAllErrors(std::move(E), [&Errors](const ErrorInfoBase &EI) {
    Errors.push_back(EI.message());
  });

  std::stringstream msg;
  std::copy(Errors.begin(), Errors.end(),
            std::ostream_iterator<std::string>(msg, "\n"));

  return msg.str();
}

/// Consume a Error without doing anything. This method should be used
/// only where an error can be considered a reasonable and expected return
/// value.
///
/// Uses of this method are potentially indicative of design problems: If it's
/// legitimate to do nothing while processing an "error", the error-producer
/// might be more clearly refactored to return an Optional<T>.
inline void consumeError(Error Err) {
  handleAllErrors(std::move(Err), [](const ErrorInfoBase &) {});
}

} // namespace llvm
