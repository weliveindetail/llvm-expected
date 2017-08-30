//===- llvm/Support/ErrorHandling.h - Fatal error handling ------*- C++ -*-===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
//
//===----------------------------------------------------------------------===//
//
// This file defines an API used to indicate fatal error conditions.  Non-fatal
// errors (most of them) should be handled through LLVMContext.
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_SUPPORT_ERRORHANDLING_H
#define LLVM_SUPPORT_ERRORHANDLING_H

#include <string>
#include <system_error>

#ifdef __GNUC__
#define EXPECTED_ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define EXPECTED_ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define EXPECTED_ATTRIBUTE_NORETURN
#endif

/// \macro EXPECTED_GNUC_PREREQ
/// \brief Extend the default __GNUC_PREREQ even if glibc's features.h isn't
/// available.
#ifndef EXPECTED_GNUC_PREREQ
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define EXPECTED_GNUC_PREREQ(maj, min, patch)                                  \
  ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >=          \
   ((maj) << 20) + ((min) << 10) + (patch))
#elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#define EXPECTED_GNUC_PREREQ(maj, min, patch)                                  \
  ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))
#else
#define EXPECTED_GNUC_PREREQ(maj, min, patch) 0
#endif
#endif

/// EXPECTED_BUILTIN_UNREACHABLE - On compilers which support it, expands
/// to an expression which states that it is undefined behavior for the
/// compiler to reach this point.  Otherwise is not defined.
#if __has_builtin(__builtin_unreachable) || EXPECTED_GNUC_PREREQ(4, 5, 0)
#define EXPECTED_BUILTIN_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define EXPECTED_BUILTIN_UNREACHABLE __assume(false)
#endif

#if _WIN32
namespace llvm {
std::error_code mapWindowsError(unsigned EV);
}
#endif

namespace llvm {

/// An error handler callback.
typedef void (*fatal_error_handler_t)(void *user_data,
                                      const std::string &reason,
                                      bool gen_crash_diag);

/// install_fatal_error_handler - Installs a new error handler to be used
/// whenever a serious (non-recoverable) error is encountered by LLVM.
///
/// If no error handler is installed the default is to print the error message
/// to stderr, and call exit(1).  If an error handler is installed then it is
/// the handler's responsibility to log the message, it will no longer be
/// printed to stderr.  If the error handler returns, then exit(1) will be
/// called.
///
/// It is dangerous to naively use an error handler which throws an exception.
/// Even though some applications desire to gracefully recover from arbitrary
/// faults, blindly throwing exceptions through unfamiliar code isn't a way to
/// achieve this.
///
/// \param user_data - An argument which will be passed to the install error
/// handler.
void install_fatal_error_handler(fatal_error_handler_t handler,
                                 void *user_data = nullptr);

/// Restores default error handling behaviour.
void remove_fatal_error_handler();

/// ScopedFatalErrorHandler - This is a simple helper class which just
/// calls install_fatal_error_handler in its constructor and
/// remove_fatal_error_handler in its destructor.
struct ScopedFatalErrorHandler {
  explicit ScopedFatalErrorHandler(fatal_error_handler_t handler,
                                   void *user_data = nullptr) {
    install_fatal_error_handler(handler, user_data);
  }

  ~ScopedFatalErrorHandler() { remove_fatal_error_handler(); }
};

/// Reports a serious error, calling any installed error handler. These
/// functions are intended to be used for error conditions which are outside
/// the control of the compiler (I/O errors, invalid user input, etc.)
///
/// If no error handler is installed the default is to print the message to
/// standard error, followed by a newline.
/// After the error handler is called this function will call exit(1), it
/// does not return.
EXPECTED_ATTRIBUTE_NORETURN void report_fatal_error(const char *reason,
                                                    bool gen_crash_diag = true);
EXPECTED_ATTRIBUTE_NORETURN void report_fatal_error(std::string reason,
                                                    bool gen_crash_diag = true);

/// This function calls abort(), and prints the optional message to stderr.
/// Use the expected_unreachable macro (that adds location info), instead of
/// calling this function directly.
EXPECTED_ATTRIBUTE_NORETURN void
expected_unreachable_internal(const char *msg = nullptr,
                              const char *file = nullptr, unsigned line = 0);
}

/// Marks that the current location is not supposed to be reachable.
/// In !NDEBUG builds, prints the message and location info to stderr.
/// In NDEBUG builds, becomes an optimizer hint that the current location
/// is not supposed to be reachable.  On compilers that don't support
/// such hints, prints a reduced message instead.
///
/// Use this instead of assert(0).  It conveys intent more clearly and
/// allows compilers to omit some unnecessary code.
#ifndef NDEBUG
#define expected_unreachable(msg)                                              \
  ::llvm::expected_unreachable_internal(msg, __FILE__, __LINE__)
#elif defined(EXPECTED_BUILTIN_UNREACHABLE)
#define expected_unreachable(msg) EXPECTED_BUILTIN_UNREACHABLE
#else
#define expected_unreachable(msg) ::llvm::expected_unreachable_internal()
#endif

#undef EXPECTED_GNUC_PREREQ

#endif // LLVM_SUPPORT_ERRORHANDLING_H
