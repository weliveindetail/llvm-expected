//===- llvm-expected/lib/ErrorBase.cpp - Recoverable error handling -------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//

#include "ErrorBase.h"
#include "Errors.h"
#include "Expected.h"

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <mutex>
#include <new>
#include <sstream>
#include <system_error>

#ifdef _MSC_VER
#include <fcntl.h>
#include <io.h>
typedef int ssize_t;
#else
#include <unistd.h>
#endif

/// EXPECTED_EXTENSION - Support compilers where we have a keyword to suppress
/// pedantic diagnostics.
#ifdef __GNUC__
#define EXPECTED_EXTENSION __extension__
#else
#define EXPECTED_EXTENSION
#endif

#if EXPECTED_ENABLE_THREADS
#define IF_THREADS(X) X
#else
#define IF_THREADS(X)
#endif

namespace {

enum class ErrorErrorCode : int { MultipleErrors = 1, InconvertibleError };

// FIXME: This class is only here to support the transition to llvm::Error. It
// will be removed once this transition is complete. Clients should prefer to
// deal with the Error value directly, rather than converting to error_code.
class ErrorErrorCategory : public std::error_category {
public:
  const char *name() const noexcept override { return "Error"; }

  std::string message(int condition) const override {
    switch (static_cast<ErrorErrorCode>(condition)) {
    case ErrorErrorCode::MultipleErrors:
      return "Multiple errors";
    case ErrorErrorCode::InconvertibleError:
      return "Inconvertible error value. An error has occurred that could "
             "not be converted to a known std::error_code. Please file a "
             "bug.";
    }
    expected_unreachable("Unhandled error code");
  }
};
}

static ErrorErrorCategory ErrorErrorCat;

static llvm::fatal_error_handler_t ErrorHandler = nullptr;
static void *ErrorHandlerUserData = nullptr;

namespace llvm {

// Mutexes to synchronize installing error handlers and calling error handlers.
// Do not use ManagedStatic, or that may allocate memory while attempting to
// report an OOM.
//
// This usage of std::mutex has to be conditionalized behind ifdefs because
// of this script:
//   compiler-rt/lib/sanitizer_common/symbolizer/scripts/build_symbolizer.sh
// That script attempts to statically link the LLVM symbolizer library with the
// STL and hide all of its symbols with 'opt -internalize'. To reduce size, it
// cuts out the threading portions of the hermetic copy of libc++ that it
// builds. We can remove these ifdefs if that script goes away.
IF_THREADS(static std::mutex ErrorHandlerMutex);

void install_fatal_error_handler(fatal_error_handler_t handler,
                                 void *user_data) {
  IF_THREADS(std::lock_guard<std::mutex> Lock(ErrorHandlerMutex));
  assert(!ErrorHandler && "Error handler already registered!\n");
  ErrorHandler = handler;
  ErrorHandlerUserData = user_data;
}

void remove_fatal_error_handler() {
  IF_THREADS(std::lock_guard<std::mutex> Lock(ErrorHandlerMutex));
  ErrorHandler = nullptr;
  ErrorHandlerUserData = nullptr;
}

void report_fatal_error(const char *Reason, bool GenCrashDiag) {
  report_fatal_error(std::string(Reason), GenCrashDiag);
}

void report_fatal_error(std::string Reason, bool GenCrashDiag) {
  llvm::fatal_error_handler_t handler = nullptr;
  void *handlerData = nullptr;
  {
    // Only acquire the mutex while reading the handler, so as not to invoke a
    // user-supplied callback under a lock.
    IF_THREADS(std::lock_guard<std::mutex> Lock(ErrorHandlerMutex));
    handler = ErrorHandler;
    handlerData = ErrorHandlerUserData;
  }

  if (handler) {
    handler(handlerData, Reason, GenCrashDiag);
  } else {
    // Blast the result out to stderr.  We don't try hard to make sure this
    // succeeds (e.g. handling EINTR) and we can't use errs() here because
    // raw ostreams can call report_fatal_error.
    char Buffer[64];
    std::ostringstream OS(Buffer);
    OS << "LLVM ERROR: " << Reason << "\n";
    std::string MessageStr = OS.str();
    ssize_t written = ::write(2, MessageStr.c_str(), MessageStr.size());
    (void)written; // If something went wrong, we deliberately just give up.
  }

  exit(1);
}

void expected_unreachable_internal(const char *msg, const char *file,
                                   unsigned line) {
  // This code intentionally doesn't call the ErrorHandler callback, because
  // llvm_unreachable is intended to be used to indicate "impossible"
  // situations, and not legitimate runtime errors.
  if (msg)
    std::cerr << msg << "\n";
  std::cerr << "UNREACHABLE executed";
  if (file)
    std::cerr << " at " << file << ":" << line;
  std::cerr << "!\n";
  abort();
#ifdef EXPECTED_BUILTIN_UNREACHABLE
  // Windows systems and possibly others don't declare abort() to be noreturn,
  // so use the unreachable builtin to avoid a Clang self-host warning.
  EXPECTED_BUILTIN_UNREACHABLE;
#endif
}

void ErrorInfoBase::anchor() {}
char ErrorInfoBase::ID = 0;
char ErrorList::ID = 0;
char ECError::ID = 0;
char StringError::ID = 0;

std::error_code ErrorList::convertToErrorCode() const {
  return std::error_code(static_cast<int>(ErrorErrorCode::MultipleErrors),
                         ErrorErrorCat);
}

std::error_code inconvertibleErrorCode() {
  return std::error_code(static_cast<int>(ErrorErrorCode::InconvertibleError),
                         ErrorErrorCat);
}

Error errorCodeToError(std::error_code EC) {
  if (!EC)
    return Error::success();
  return Error(std::make_unique<ECError>(ECError(EC)));
}

std::error_code errorToErrorCode(Error Err) {
  std::error_code EC;
  handleAllErrors(std::move(Err), [&](const ErrorInfoBase &EI) {
    EC = EI.convertToErrorCode();
  });
  if (EC == inconvertibleErrorCode())
    report_fatal_error(EC.message());
  return EC;
}

StringError::StringError(std::string Msg, std::error_code EC)
    : Msg(std::move(Msg)), EC(EC) {}

void StringError::log(std::ostream &OS) const { OS << Msg; }

std::error_code StringError::convertToErrorCode() const { return EC; }

void logAllUnhandledErrors(Error E, std::ostream &OS, std::string ErrorBanner) {
  if (!E)
    return;
  OS << ErrorBanner;
  handleAllErrors(std::move(E), [&](const ErrorInfoBase &EI) {
    EI.log(OS);
    OS << "\n";
  });
}

void report_fatal_error(Error Err, bool GenCrashDiag) {
  assert(Err && "report_fatal_error called with success value");
  std::string ErrMsg;
  {
    std::ostringstream ErrStream(ErrMsg);
    logAllUnhandledErrors(std::move(Err), ErrStream, "");
  }
  report_fatal_error(ErrMsg);
}

#ifdef _WIN32

#include <winerror.h>

// I'd rather not double the line count of the following.
#define MAP_ERR_TO_COND(x, y)                                                  \
  case x:                                                                      \
    return std::make_error_code(std::errc::y)

/// Useful helper for Windows system errors:
///
///   @code{.cpp}
///   std::error_code ec = llvm::mapWindowsError(::GetLastError());
///   @endcode
std::error_code mapWindowsError(unsigned EV) {
  switch (EV) {
    MAP_ERR_TO_COND(ERROR_ACCESS_DENIED, permission_denied);
    MAP_ERR_TO_COND(ERROR_ALREADY_EXISTS, file_exists);
    MAP_ERR_TO_COND(ERROR_BAD_UNIT, no_such_device);
    MAP_ERR_TO_COND(ERROR_BUFFER_OVERFLOW, filename_too_long);
    MAP_ERR_TO_COND(ERROR_BUSY, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_BUSY_DRIVE, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_CANNOT_MAKE, permission_denied);
    MAP_ERR_TO_COND(ERROR_CANTOPEN, io_error);
    MAP_ERR_TO_COND(ERROR_CANTREAD, io_error);
    MAP_ERR_TO_COND(ERROR_CANTWRITE, io_error);
    MAP_ERR_TO_COND(ERROR_CURRENT_DIRECTORY, permission_denied);
    MAP_ERR_TO_COND(ERROR_DEV_NOT_EXIST, no_such_device);
    MAP_ERR_TO_COND(ERROR_DEVICE_IN_USE, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_DIR_NOT_EMPTY, directory_not_empty);
    MAP_ERR_TO_COND(ERROR_DIRECTORY, invalid_argument);
    MAP_ERR_TO_COND(ERROR_DISK_FULL, no_space_on_device);
    MAP_ERR_TO_COND(ERROR_FILE_EXISTS, file_exists);
    MAP_ERR_TO_COND(ERROR_FILE_NOT_FOUND, no_such_file_or_directory);
    MAP_ERR_TO_COND(ERROR_HANDLE_DISK_FULL, no_space_on_device);
    MAP_ERR_TO_COND(ERROR_INVALID_ACCESS, permission_denied);
    MAP_ERR_TO_COND(ERROR_INVALID_DRIVE, no_such_device);
    MAP_ERR_TO_COND(ERROR_INVALID_FUNCTION, function_not_supported);
    MAP_ERR_TO_COND(ERROR_INVALID_HANDLE, invalid_argument);
    MAP_ERR_TO_COND(ERROR_INVALID_NAME, invalid_argument);
    MAP_ERR_TO_COND(ERROR_LOCK_VIOLATION, no_lock_available);
    MAP_ERR_TO_COND(ERROR_LOCKED, no_lock_available);
    MAP_ERR_TO_COND(ERROR_NEGATIVE_SEEK, invalid_argument);
    MAP_ERR_TO_COND(ERROR_NOACCESS, permission_denied);
    MAP_ERR_TO_COND(ERROR_NOT_ENOUGH_MEMORY, not_enough_memory);
    MAP_ERR_TO_COND(ERROR_NOT_READY, resource_unavailable_try_again);
    MAP_ERR_TO_COND(ERROR_OPEN_FAILED, io_error);
    MAP_ERR_TO_COND(ERROR_OPEN_FILES, device_or_resource_busy);
    MAP_ERR_TO_COND(ERROR_OUTOFMEMORY, not_enough_memory);
    MAP_ERR_TO_COND(ERROR_PATH_NOT_FOUND, no_such_file_or_directory);
    MAP_ERR_TO_COND(ERROR_BAD_NETPATH, no_such_file_or_directory);
    MAP_ERR_TO_COND(ERROR_READ_FAULT, io_error);
    MAP_ERR_TO_COND(ERROR_RETRY, resource_unavailable_try_again);
    MAP_ERR_TO_COND(ERROR_SEEK, io_error);
    MAP_ERR_TO_COND(ERROR_SHARING_VIOLATION, permission_denied);
    MAP_ERR_TO_COND(ERROR_TOO_MANY_OPEN_FILES, too_many_files_open);
    MAP_ERR_TO_COND(ERROR_WRITE_FAULT, io_error);
    MAP_ERR_TO_COND(ERROR_WRITE_PROTECT, permission_denied);
    MAP_ERR_TO_COND(WSAEACCES, permission_denied);
    MAP_ERR_TO_COND(WSAEBADF, bad_file_descriptor);
    MAP_ERR_TO_COND(WSAEFAULT, bad_address);
    MAP_ERR_TO_COND(WSAEINTR, interrupted);
    MAP_ERR_TO_COND(WSAEINVAL, invalid_argument);
    MAP_ERR_TO_COND(WSAEMFILE, too_many_files_open);
    MAP_ERR_TO_COND(WSAENAMETOOLONG, filename_too_long);
  default:
    return std::error_code(EV, std::system_category());
  }
}

#endif

} // end namespace llvm
