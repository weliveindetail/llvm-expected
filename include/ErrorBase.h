//===- llvm-expected/lib/ErrorBase.h - Recoverable error handling ---------===//
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

#pragma once

#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <memory>
#include <new>
#include <sstream>
#include <string>
#include <system_error>
#include <type_traits>
#include <utility>
#include <vector>

#ifndef __has_feature
# define __has_feature(x) 0
#endif

#ifndef __has_cpp_attribute
# define __has_cpp_attribute(x) 0
#endif

#ifndef __has_builtin
# define __has_builtin(x) 0
#endif

/* Define to 1 if you have the <unistd.h> header file. */
#ifndef _WIN32
#define EXPECTED_HAVE_UNISTD_H 1
#endif

/* Define if threads enabled */
#define EXPECTED_ENABLE_THREADS 1

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

/// \macro EXPECTED_ALIGNAS
/// \brief Used to specify a minimum alignment for a structure or variable.
#if __GNUC__ && !__has_feature(cxx_alignas) && !EXPECTED_GNUC_PREREQ(4, 8, 1)
#define EXPECTED_ALIGNAS(x) __attribute__((aligned(x)))
#else
#define EXPECTED_ALIGNAS(x) alignas(x)
#endif

/// EXPECTED_BUILTIN_UNREACHABLE - On compilers which support it, expands
/// to an expression which states that it is undefined behavior for the
/// compiler to reach this point.  Otherwise is not defined.
#if __has_builtin(__builtin_unreachable) || EXPECTED_GNUC_PREREQ(4, 5, 0)
#define EXPECTED_BUILTIN_UNREACHABLE __builtin_unreachable()
#elif defined(_MSC_VER)
#define EXPECTED_BUILTIN_UNREACHABLE __assume(false)
#endif

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

/// EXPECTED_NODISCARD - Warn if a type or return value is discarded.
#if __cplusplus > 201402L && __has_cpp_attribute(nodiscard)
#define EXPECTED_NODISCARD [[nodiscard]]
#elif !__cplusplus
// Workaround for llvm.org/PR23435, since clang 3.6 and below emit a spurious
// error when __has_cpp_attribute is given a scoped attribute in C mode.
#define EXPECTED_NODISCARD
#elif __has_cpp_attribute(clang::warn_unused_result)
#define EXPECTED_NODISCARD [[clang::warn_unused_result]]
#else
#define EXPECTED_NODISCARD
#endif

#ifdef __GNUC__
#define EXPECTED_ATTRIBUTE_NORETURN __attribute__((noreturn))
#elif defined(_MSC_VER)
#define EXPECTED_ATTRIBUTE_NORETURN __declspec(noreturn)
#else
#define EXPECTED_ATTRIBUTE_NORETURN
#endif

namespace llvm {

/// \struct AlignedCharArray
/// \brief Helper for building an aligned character array type.
///
/// This template is used to explicitly build up a collection of aligned
/// character array types. We have to build these up using a macro and explicit
/// specialization to cope with MSVC (at least till 2015) where only an
/// integer literal can be used to specify an alignment constraint. Once built
/// up here, we can then begin to indirect between these using normal C++
/// template parameters.

// MSVC requires special handling here.
#ifndef _MSC_VER

template <std::size_t Alignment, std::size_t Size> struct AlignedCharArray {
  EXPECTED_ALIGNAS(Alignment) char buffer[Size];
};

#else // _MSC_VER

/// \brief Create a type with an aligned char buffer.
template <std::size_t Alignment, std::size_t Size> struct AlignedCharArray;

// We provide special variations of this template for the most common
// alignments because __declspec(align(...)) doesn't actually work when it is
// a member of a by-value function argument in MSVC, even if the alignment
// request is something reasonably like 8-byte or 16-byte. Note that we can't
// even include the declspec with the union that forces the alignment because
// MSVC warns on the existence of the declspec despite the union member forcing
// proper alignment.

template <std::size_t Size> struct AlignedCharArray<1, Size> {
  union {
    char aligned;
    char buffer[Size];
  };
};

template <std::size_t Size> struct AlignedCharArray<2, Size> {
  union {
    short aligned;
    char buffer[Size];
  };
};

template <std::size_t Size> struct AlignedCharArray<4, Size> {
  union {
    int aligned;
    char buffer[Size];
  };
};

template <std::size_t Size> struct AlignedCharArray<8, Size> {
  union {
    double aligned;
    char buffer[Size];
  };
};

// The rest of these are provided with a __declspec(align(...)) and we simply
// can't pass them by-value as function arguments on MSVC.

#define EXPECTED_ALIGNEDCHARARRAY_TEMPLATE_ALIGNMENT(x)                        \
  template <std::size_t Size> struct AlignedCharArray<x, Size> {               \
    __declspec(align(x)) char buffer[Size];                                    \
  };

EXPECTED_ALIGNEDCHARARRAY_TEMPLATE_ALIGNMENT(16)
EXPECTED_ALIGNEDCHARARRAY_TEMPLATE_ALIGNMENT(32)
EXPECTED_ALIGNEDCHARARRAY_TEMPLATE_ALIGNMENT(64)
EXPECTED_ALIGNEDCHARARRAY_TEMPLATE_ALIGNMENT(128)

#undef EXPECTED_ALIGNEDCHARARRAY_TEMPLATE_ALIGNMENT

#endif // _MSC_VER

namespace detail {
template <typename T1, typename T2 = char, typename T3 = char,
          typename T4 = char, typename T5 = char, typename T6 = char,
          typename T7 = char, typename T8 = char, typename T9 = char,
          typename T10 = char>
class AlignerImpl {
  T1 t1; T2 t2; T3 t3; T4 t4; T5 t5; T6 t6; T7 t7; T8 t8; T9 t9; T10 t10;

  AlignerImpl() = delete;
};

template <typename T1, typename T2 = char, typename T3 = char,
          typename T4 = char, typename T5 = char, typename T6 = char,
          typename T7 = char, typename T8 = char, typename T9 = char,
          typename T10 = char>
union SizerImpl {
  char arr1[sizeof(T1)], arr2[sizeof(T2)], arr3[sizeof(T3)], arr4[sizeof(T4)],
      arr5[sizeof(T5)], arr6[sizeof(T6)], arr7[sizeof(T7)], arr8[sizeof(T8)],
      arr9[sizeof(T9)], arr10[sizeof(T10)];
};
} // end namespace detail

/// \brief This union template exposes a suitably aligned and sized character
/// array member which can hold elements of any of up to ten types.
///
/// These types may be arrays, structs, or any other types. The goal is to
/// expose a char array buffer member which can be used as suitable storage for
/// a placement new of any of these types. Support for more than ten types can
/// be added at the cost of more boilerplate.
template <typename T1, typename T2 = char, typename T3 = char,
          typename T4 = char, typename T5 = char, typename T6 = char,
          typename T7 = char, typename T8 = char, typename T9 = char,
          typename T10 = char>
struct AlignedCharArrayUnion
    : llvm::AlignedCharArray<alignof(llvm::detail::AlignerImpl<
                                     T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>),
                             sizeof(::llvm::detail::SizerImpl<
                                    T1, T2, T3, T4, T5, T6, T7, T8, T9, T10>)> {
};

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

/// This function calls abort(), and prints the optional message to stderr.
/// Use the expected_unreachable macro (that adds location info), instead of
/// calling this function directly.
EXPECTED_ATTRIBUTE_NORETURN void
expected_unreachable_internal(const char *msg = nullptr,
                              const char *file = nullptr, unsigned line = 0);

class ErrorSuccess;

/// Base class for error info classes. Do not extend this directly: Extend
/// the ErrorInfo template subclass instead.
class ErrorInfoBase {
public:
  virtual ~ErrorInfoBase() = default;

  /// Print an error message to an output stream.
  virtual void log(std::ostream &OS) const = 0;

  /// Return the error message as a string.
  virtual std::string message() const {
    std::string Msg;
    std::ostringstream OS(Msg);
    log(OS);
    return OS.str();
  }

  /// Convert this error to a std::error_code.
  ///
  /// This is a temporary crutch to enable interaction with code still
  /// using std::error_code. It will be removed in the future.
  virtual std::error_code convertToErrorCode() const = 0;

  // Returns the class ID for this type.
  static const void *classID() { return &ID; }

  // Returns the class ID for the dynamic type of this ErrorInfoBase instance.
  virtual const void *dynamicClassID() const = 0;

  // Check whether this instance is a subclass of the class identified by
  // ClassID.
  virtual bool isA(const void *const ClassID) const {
    return ClassID == classID();
  }

  // Check whether this instance is a subclass of ErrorInfoT.
  template <typename ErrorInfoT> bool isA() const {
    return isA(ErrorInfoT::classID());
  }

private:
  virtual void anchor();

  static char ID;
};

/// Lightweight error class with error context and mandatory checking.
///
/// Instances of this class wrap a ErrorInfoBase pointer. Failure states
/// are represented by setting the pointer to a ErrorInfoBase subclass
/// instance containing information describing the failure. Success is
/// represented by a null pointer value.
///
/// Instances of Error also contains a 'Checked' flag, which must be set
/// before the destructor is called, otherwise the destructor will trigger a
/// runtime error. This enforces at runtime the requirement that all Error
/// instances be checked or returned to the caller.
///
/// There are two ways to set the checked flag, depending on what state the
/// Error instance is in. For Error instances indicating success, it
/// is sufficient to invoke the boolean conversion operator. E.g.:
///
///   @code{.cpp}
///   Error foo(<...>);
///
///   if (auto E = foo(<...>))
///     return E; // <- Return E if it is in the error state.
///   // We have verified that E was in the success state. It can now be safely
///   // destroyed.
///   @endcode
///
/// A success value *can not* be dropped. For example, just calling 'foo(<...>)'
/// without testing the return value will raise a runtime error, even if foo
/// returns success.
///
/// For Error instances representing failure, you must use either the
/// handleErrors or handleAllErrors function with a typed handler. E.g.:
///
///   @code{.cpp}
///   class MyErrorInfo : public ErrorInfo<MyErrorInfo> {
///     // Custom error info.
///   };
///
///   Error foo(<...>) { return make_error<MyErrorInfo>(...); }
///
///   auto E = foo(<...>); // <- foo returns failure with MyErrorInfo.
///   auto NewE =
///     handleErrors(E,
///       [](const MyErrorInfo &M) {
///         // Deal with the error.
///       },
///       [](std::unique_ptr<OtherError> M) -> Error {
///         if (canHandle(*M)) {
///           // handle error.
///           return Error::success();
///         }
///         // Couldn't handle this error instance. Pass it up the stack.
///         return Error(std::move(M));
///       );
///   // Note - we must check or return NewE in case any of the handlers
///   // returned a new error.
///   @endcode
///
/// The handleAllErrors function is identical to handleErrors, except
/// that it has a void return type, and requires all errors to be handled and
/// no new errors be returned. It prevents errors (assuming they can all be
/// handled) from having to be bubbled all the way to the top-level.
///
/// *All* Error instances must be checked before destruction, even if
/// they're moved-assigned or constructed from Success values that have already
/// been checked. This enforces checking through all levels of the call stack.
class EXPECTED_NODISCARD Error {
  // ErrorList needs to be able to yank ErrorInfoBase pointers out of this
  // class to add to the error list.
  friend class ErrorList;

  // handleErrors needs to be able to set the Checked flag.
  template <typename... HandlerTs>
  friend Error handleErrors(Error E, HandlerTs &&... Handlers);

  // Expected<T> needs to be able to steal the payload when constructed from an
  // error.
  template <typename T> friend class Expected;

protected:
  /// Create a success value. Prefer using 'Error::success()' for readability
  Error() {
    setPtr(nullptr);
    setChecked(false);
  }

public:
  /// Create a success value.
  static ErrorSuccess success();

  // Errors are not copy-constructable.
  Error(const Error &Other) = delete;

  /// Move-construct an error value. The newly constructed error is considered
  /// unchecked, even if the source error had been checked. The original error
  /// becomes a checked Success value, regardless of its original state.
  Error(Error &&Other) {
    setChecked(true);
    *this = std::move(Other);
  }

  /// Create an error value. Prefer using the 'make_error' function, but
  /// this constructor can be useful when "re-throwing" errors from handlers.
  Error(std::unique_ptr<ErrorInfoBase> Payload) {
    setPtr(Payload.release());
    setChecked(false);
  }

  // Errors are not copy-assignable.
  Error &operator=(const Error &Other) = delete;

  /// Move-assign an error value. The current error must represent success, you
  /// you cannot overwrite an unhandled error. The current error is then
  /// considered unchecked. The source error becomes a checked success value,
  /// regardless of its original state.
  Error &operator=(Error &&Other) {
    // Don't allow overwriting of unchecked values.
    assertIsChecked();
    setPtr(Other.getPtr());

    // This Error is unchecked, even if the source error was checked.
    setChecked(false);

    // Null out Other's payload and set its checked bit.
    Other.setPtr(nullptr);
    Other.setChecked(true);

    return *this;
  }

  /// Destroy a Error. Fails with a call to abort() if the error is
  /// unchecked.
  ~Error() {
    assertIsChecked();
    delete getPtr();
  }

  /// Bool conversion. Returns true if this Error is in a failure state,
  /// and false if it is in an accept state. If the error is in a Success state
  /// it will be considered checked.
  explicit operator bool() {
    setChecked(getPtr() == nullptr);
    return getPtr() != nullptr;
  }

  /// Check whether one error is a subclass of another.
  template <typename ErrT> bool isA() const {
    return getPtr() && getPtr()->isA(ErrT::classID());
  }

  /// Returns the dynamic class id of this error, or null if this is a success
  /// value.
  const void *dynamicClassID() const {
    if (!getPtr())
      return nullptr;
    return getPtr()->dynamicClassID();
  }

private:
  void assertIsChecked() {
#ifndef NDEBUG
    if (!getChecked() || getPtr()) {
      std::cerr << "Program aborted due to an unhandled Error:\n";
      if (getPtr())
        getPtr()->log(std::cerr);
      else
        std::cerr << "Error value was Success. "
                     "Note: Success values must still be "
                     "checked prior to being destroyed.\n";
      abort();
    }
#endif
  }

  ErrorInfoBase *getPtr() const {
#ifndef NDEBUG
    return reinterpret_cast<ErrorInfoBase *>(
        reinterpret_cast<uintptr_t>(Payload) & ~static_cast<uintptr_t>(0x1));
#else
    return Payload;
#endif
  }

  void setPtr(ErrorInfoBase *EI) {
#ifndef NDEBUG
    Payload = reinterpret_cast<ErrorInfoBase *>(
        (reinterpret_cast<uintptr_t>(EI) & ~static_cast<uintptr_t>(0x1)) |
        (reinterpret_cast<uintptr_t>(Payload) & 0x1));
#else
    Payload = EI;
#endif
  }

  bool getChecked() const {
#ifndef NDEBUG
    return (reinterpret_cast<uintptr_t>(Payload) & 0x1) == 0;
#else
    return true;
#endif
  }

  void setChecked(bool V) {
#ifndef NDEBUG
    Payload = reinterpret_cast<ErrorInfoBase *>(
        (reinterpret_cast<uintptr_t>(Payload) & ~static_cast<uintptr_t>(0x1)) |
        (V ? 0 : 1));
#else
    (void)V;
#endif
  }

  std::unique_ptr<ErrorInfoBase> takePayload() {
    std::unique_ptr<ErrorInfoBase> Tmp(getPtr());
    setPtr(nullptr);
    setChecked(true);
    return Tmp;
  }

  ErrorInfoBase *Payload = nullptr;
};

/// Subclass of Error for the sole purpose of identifying the success path in
/// the type system. This allows to catch invalid conversion to Expected<T> at
/// compile time.
class ErrorSuccess : public Error {};

inline ErrorSuccess Error::success() { return ErrorSuccess(); }

/// Make a Error instance representing failure using the given error info
/// type.
template <typename ErrT, typename... ArgTs> Error make_error(ArgTs &&... Args) {
  return Error(std::make_unique<ErrT>(std::forward<ArgTs>(Args)...));
}

/// Base class for user error types. Users should declare their error types
/// like:
///
/// class MyError : public ErrorInfo<MyError> {
///   ....
/// };
///
/// This class provides an implementation of the ErrorInfoBase::kind
/// method, which is used by the Error RTTI system.
template <typename ThisErrT, typename ParentErrT = ErrorInfoBase>
class ErrorInfo : public ParentErrT {
public:
  static const void *classID() { return &ThisErrT::ID; }

  const void *dynamicClassID() const override { return &ThisErrT::ID; }

  bool isA(const void *const ClassID) const override {
    return ClassID == classID() || ParentErrT::isA(ClassID);
  }
};

/// Helper for testing applicability of, and applying, handlers for
/// ErrorInfo types.
template <typename HandlerT>
class ErrorHandlerTraits
    : public ErrorHandlerTraits<decltype(
          &std::remove_reference<HandlerT>::type::operator())> {};

// Specialization functions of the form 'Error (const ErrT&)'.
template <typename ErrT> class ErrorHandlerTraits<Error (&)(ErrT &)> {
public:
  static bool appliesTo(const ErrorInfoBase &E) {
    return E.template isA<ErrT>();
  }

  template <typename HandlerT>
  static Error apply(HandlerT &&H, std::unique_ptr<ErrorInfoBase> E) {
    assert(appliesTo(*E) && "Applying incorrect handler");
    return H(static_cast<ErrT &>(*E));
  }
};

// Specialization functions of the form 'void (const ErrT&)'.
template <typename ErrT> class ErrorHandlerTraits<void (&)(ErrT &)> {
public:
  static bool appliesTo(const ErrorInfoBase &E) {
    return E.template isA<ErrT>();
  }

  template <typename HandlerT>
  static Error apply(HandlerT &&H, std::unique_ptr<ErrorInfoBase> E) {
    assert(appliesTo(*E) && "Applying incorrect handler");
    H(static_cast<ErrT &>(*E));
    return Error::success();
  }
};

/// Specialization for functions of the form 'Error (std::unique_ptr<ErrT>)'.
template <typename ErrT>
class ErrorHandlerTraits<Error (&)(std::unique_ptr<ErrT>)> {
public:
  static bool appliesTo(const ErrorInfoBase &E) {
    return E.template isA<ErrT>();
  }

  template <typename HandlerT>
  static Error apply(HandlerT &&H, std::unique_ptr<ErrorInfoBase> E) {
    assert(appliesTo(*E) && "Applying incorrect handler");
    std::unique_ptr<ErrT> SubE(static_cast<ErrT *>(E.release()));
    return H(std::move(SubE));
  }
};

/// Specialization for functions of the form 'Error (std::unique_ptr<ErrT>)'.
template <typename ErrT>
class ErrorHandlerTraits<void (&)(std::unique_ptr<ErrT>)> {
public:
  static bool appliesTo(const ErrorInfoBase &E) {
    return E.template isA<ErrT>();
  }

  template <typename HandlerT>
  static Error apply(HandlerT &&H, std::unique_ptr<ErrorInfoBase> E) {
    assert(appliesTo(*E) && "Applying incorrect handler");
    std::unique_ptr<ErrT> SubE(static_cast<ErrT *>(E.release()));
    H(std::move(SubE));
    return Error::success();
  }
};

// Specialization for member functions of the form 'RetT (const ErrT&)'.
template <typename C, typename RetT, typename ErrT>
class ErrorHandlerTraits<RetT (C::*)(ErrT &)>
    : public ErrorHandlerTraits<RetT (&)(ErrT &)> {};

// Specialization for member functions of the form 'RetT (const ErrT&) const'.
template <typename C, typename RetT, typename ErrT>
class ErrorHandlerTraits<RetT (C::*)(ErrT &) const>
    : public ErrorHandlerTraits<RetT (&)(ErrT &)> {};

// Specialization for member functions of the form 'RetT (const ErrT&)'.
template <typename C, typename RetT, typename ErrT>
class ErrorHandlerTraits<RetT (C::*)(const ErrT &)>
    : public ErrorHandlerTraits<RetT (&)(ErrT &)> {};

// Specialization for member functions of the form 'RetT (const ErrT&) const'.
template <typename C, typename RetT, typename ErrT>
class ErrorHandlerTraits<RetT (C::*)(const ErrT &) const>
    : public ErrorHandlerTraits<RetT (&)(ErrT &)> {};

/// Specialization for member functions of the form
/// 'RetT (std::unique_ptr<ErrT>) const'.
template <typename C, typename RetT, typename ErrT>
class ErrorHandlerTraits<RetT (C::*)(std::unique_ptr<ErrT>)>
    : public ErrorHandlerTraits<RetT (&)(std::unique_ptr<ErrT>)> {};

/// Specialization for member functions of the form
/// 'RetT (std::unique_ptr<ErrT>) const'.
template <typename C, typename RetT, typename ErrT>
class ErrorHandlerTraits<RetT (C::*)(std::unique_ptr<ErrT>) const>
    : public ErrorHandlerTraits<RetT (&)(std::unique_ptr<ErrT>)> {};

inline Error handleErrorImpl(std::unique_ptr<ErrorInfoBase> Payload) {
  return Error(std::move(Payload));
}

template <typename HandlerT, typename... HandlerTs>
Error handleErrorImpl(std::unique_ptr<ErrorInfoBase> Payload,
                      HandlerT &&Handler, HandlerTs &&... Handlers) {
  if (ErrorHandlerTraits<HandlerT>::appliesTo(*Payload))
    return ErrorHandlerTraits<HandlerT>::apply(std::forward<HandlerT>(Handler),
                                               std::move(Payload));
  return handleErrorImpl(std::move(Payload),
                         std::forward<HandlerTs>(Handlers)...);
}

/// Helper for Errors used as out-parameters.
///
/// This helper is for use with the Error-as-out-parameter idiom, where an error
/// is passed to a function or method by reference, rather than being returned.
/// In such cases it is helpful to set the checked bit on entry to the function
/// so that the error can be written to (unchecked Errors abort on assignment)
/// and clear the checked bit on exit so that clients cannot accidentally forget
/// to check the result. This helper performs these actions automatically using
/// RAII:
///
///   @code{.cpp}
///   Result foo(Error &Err) {
///     ErrorAsOutParameter ErrAsOutParam(&Err); // 'Checked' flag set
///     // <body of foo>
///     // <- 'Checked' flag auto-cleared when ErrAsOutParam is destructed.
///   }
///   @endcode
///
/// ErrorAsOutParameter takes an Error* rather than Error& so that it can be
/// used with optional Errors (Error pointers that are allowed to be null). If
/// ErrorAsOutParameter took an Error reference, an instance would have to be
/// created inside every condition that verified that Error was non-null. By
/// taking an Error pointer we can just create one instance at the top of the
/// function.
class ErrorAsOutParameter {
public:
  ErrorAsOutParameter(Error *Err) : Err(Err) {
    // Raise the checked bit if Err is success.
    if (Err)
      (void)!!*Err;
  }

  ~ErrorAsOutParameter() {
    // Clear the checked bit.
    if (Err && !*Err)
      *Err = Error::success();
  }

private:
  Error *Err;
};

/// Report a fatal error if Err is a failure value.
///
/// This function can be used to wrap calls to fallible functions ONLY when it
/// is known that the Error will always be a success value. E.g.
///
///   @code{.cpp}
///   // foo only attempts the fallible operation if DoFallibleOperation is
///   // true. If DoFallibleOperation is false then foo always returns
///   // Error::success().
///   Error foo(bool DoFallibleOperation);
///
///   cantFail(foo(false));
///   @endcode
inline void cantFail(Error Err, const char *Msg = nullptr) {
  if (Err) {
    if (!Msg)
      Msg = "Failure value returned from cantFail wrapped call";
    expected_unreachable(Msg);
  }
}

/// \brief Stores a reference that can be changed.
template <typename T> class ReferenceStorage {
  T *Storage;

public:
  ReferenceStorage(T &Ref) : Storage(&Ref) {}

  operator T &() const { return *Storage; }
  T &get() const { return *Storage; }
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
EXPECTED_ATTRIBUTE_NORETURN void report_fatal_error(Error Err,
                                                    bool gen_crash_diag = true);

#if _WIN32
std::error_code mapWindowsError(unsigned EV);
#endif

} // end namespace llvm

#undef EXPECTED_ALIGNAS
#undef EXPECTED_GNUC_PREREQ
