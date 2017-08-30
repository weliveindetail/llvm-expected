//===- llvm/Support/Error.h - Recoverable error handling --------*- C++ -*-===//
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

#include <cassert>
#include <memory>
#include <type_traits>

#include "ErrorBase.h"

namespace llvm {

/// Tagged union holding either a T or a Error.
///
/// This class parallels ErrorOr, but replaces error_code with Error. Since
/// Error cannot be copied, this class replaces getError() with
/// takeError(). It also adds an bool errorIsA<ErrT>() method for testing the
/// error class type.
template <class T> class EXPECTED_NODISCARD Expected {
  template <class T1> friend class ExpectedAsOutParameter;
  template <class OtherT> friend class Expected;

  static const bool isRef = std::is_reference<T>::value;

  using wrap = ReferenceStorage<typename std::remove_reference<T>::type>;

  using error_type = std::unique_ptr<ErrorInfoBase>;

public:
  using storage_type = typename std::conditional<isRef, wrap, T>::type;
  using value_type = T;

private:
  using reference = typename std::remove_reference<T>::type &;
  using const_reference = const typename std::remove_reference<T>::type &;
  using pointer = typename std::remove_reference<T>::type *;
  using const_pointer = const typename std::remove_reference<T>::type *;

public:
  /// Create an Expected<T> error value from the given Error.
  Expected(Error Err)
      : HasError(true)
#ifndef NDEBUG
        // Expected is unchecked upon construction in Debug builds.
        ,
        Unchecked(true)
#endif
  {
    assert(Err && "Cannot create Expected<T> from Error success value.");
    new (getErrorStorage()) error_type(Err.takePayload());
  }

  /// Forbid to convert from Error::success() implicitly, this avoids having
  /// Expected<T> foo() { return Error::success(); } which compiles otherwise
  /// but triggers the assertion above.
  Expected(ErrorSuccess) = delete;

  /// Create an Expected<T> success value from the given OtherT value, which
  /// must be convertible to T.
  template <typename OtherT>
  Expected(OtherT &&Val,
           typename std::enable_if<std::is_convertible<OtherT, T>::value>::type
               * = nullptr)
      : HasError(false)
#ifndef NDEBUG
        // Expected is unchecked upon construction in Debug builds.
        ,
        Unchecked(true)
#endif
  {
    new (getStorage()) storage_type(std::forward<OtherT>(Val));
  }

  /// Move construct an Expected<T> value.
  Expected(Expected &&Other) { moveConstruct(std::move(Other)); }

  /// Move construct an Expected<T> value from an Expected<OtherT>, where OtherT
  /// must be convertible to T.
  template <class OtherT>
  Expected(Expected<OtherT> &&Other,
           typename std::enable_if<std::is_convertible<OtherT, T>::value>::type
               * = nullptr) {
    moveConstruct(std::move(Other));
  }

  /// Move construct an Expected<T> value from an Expected<OtherT>, where OtherT
  /// isn't convertible to T.
  template <class OtherT>
  explicit Expected(
      Expected<OtherT> &&Other,
      typename std::enable_if<!std::is_convertible<OtherT, T>::value>::type * =
          nullptr) {
    moveConstruct(std::move(Other));
  }

  /// Move-assign from another Expected<T>.
  Expected &operator=(Expected &&Other) {
    moveAssign(std::move(Other));
    return *this;
  }

  /// Destroy an Expected<T>.
  ~Expected() {
    assertIsChecked();
    if (!HasError)
      getStorage()->~storage_type();
    else
      getErrorStorage()->~error_type();
  }

  /// \brief Return false if there is an error.
  explicit operator bool() {
#ifndef NDEBUG
    Unchecked = HasError;
#endif
    return !HasError;
  }

  /// \brief Returns a reference to the stored T value.
  reference get() {
    assertIsChecked();
    return *getStorage();
  }

  /// \brief Returns a const reference to the stored T value.
  const_reference get() const {
    assertIsChecked();
    return const_cast<Expected<T> *>(this)->get();
  }

  /// \brief Check that this Expected<T> is an error of type ErrT.
  template <typename ErrT> bool errorIsA() const {
    return HasError && (*getErrorStorage())->template isA<ErrT>();
  }

  /// \brief Take ownership of the stored error.
  /// After calling this the Expected<T> is in an indeterminate state that can
  /// only be safely destructed. No further calls (beside the destructor) should
  /// be made on the Expected<T> vaule.
  Error takeError() {
#ifndef NDEBUG
    Unchecked = false;
#endif
    return HasError ? Error(std::move(*getErrorStorage())) : Error::success();
  }

  /// \brief Returns a pointer to the stored T value.
  pointer operator->() {
    assertIsChecked();
    return toPointer(getStorage());
  }

  /// \brief Returns a const pointer to the stored T value.
  const_pointer operator->() const {
    assertIsChecked();
    return toPointer(getStorage());
  }

  /// \brief Returns a reference to the stored T value.
  reference operator*() {
    assertIsChecked();
    return *getStorage();
  }

  /// \brief Returns a const reference to the stored T value.
  const_reference operator*() const {
    assertIsChecked();
    return *getStorage();
  }

private:
  template <class T1>
  static bool compareThisIfSameType(const T1 &a, const T1 &b) {
    return &a == &b;
  }

  template <class T1, class T2>
  static bool compareThisIfSameType(const T1 &a, const T2 &b) {
    return false;
  }

  template <class OtherT> void moveConstruct(Expected<OtherT> &&Other) {
    HasError = Other.HasError;
#ifndef NDEBUG
    Unchecked = true;
    Other.Unchecked = false;
#endif

    if (!HasError)
      new (getStorage()) storage_type(std::move(*Other.getStorage()));
    else
      new (getErrorStorage()) error_type(std::move(*Other.getErrorStorage()));
  }

  template <class OtherT> void moveAssign(Expected<OtherT> &&Other) {
    assertIsChecked();

    if (compareThisIfSameType(*this, Other))
      return;

    this->~Expected();
    new (this) Expected(std::move(Other));
  }

  pointer toPointer(pointer Val) { return Val; }

  const_pointer toPointer(const_pointer Val) const { return Val; }

  pointer toPointer(wrap *Val) { return &Val->get(); }

  const_pointer toPointer(const wrap *Val) const { return &Val->get(); }

  storage_type *getStorage() {
    assert(!HasError && "Cannot get value when an error exists!");
    return reinterpret_cast<storage_type *>(TStorage.buffer);
  }

  const storage_type *getStorage() const {
    assert(!HasError && "Cannot get value when an error exists!");
    return reinterpret_cast<const storage_type *>(TStorage.buffer);
  }

  error_type *getErrorStorage() {
    assert(HasError && "Cannot get error when a value exists!");
    return reinterpret_cast<error_type *>(ErrorStorage.buffer);
  }

  const error_type *getErrorStorage() const {
    assert(HasError && "Cannot get error when a value exists!");
    return reinterpret_cast<const error_type *>(ErrorStorage.buffer);
  }

  // Used by ExpectedAsOutParameter to reset the checked flag.
  void setUnchecked() {
#ifndef NDEBUG
    Unchecked = true;
#endif
  }

  void assertIsChecked() {
#ifndef NDEBUG
    if (Unchecked) {
      std::cerr
          << "Expected<T> must be checked before access or destruction.\n";
      if (HasError) {
        std::cerr << "Unchecked Expected<T> contained error:\n";
        (*getErrorStorage())->log(std::cerr);
      } else
        std::cerr
            << "Expected<T> value was in success state. (Note: Expected<T> "
               "values in success mode must still be checked prior to being "
               "destroyed).\n";
      abort();
    }
#endif
  }

  union {
    AlignedCharArrayUnion<storage_type> TStorage;
    AlignedCharArrayUnion<error_type> ErrorStorage;
  };
  bool HasError : 1;
#ifndef NDEBUG
  bool Unchecked : 1;
#endif
};

void logAllUnhandledErrors(Error E, std::ostream &OS, std::string ErrorBanner);

/// Helper for check-and-exit error handling.
///
/// For tool use only. NOT FOR USE IN LIBRARY CODE.
///
class ExitOnError {
public:
  /// Create an error on exit helper.
  ExitOnError(std::string Banner = "", int DefaultErrorExitCode = 1)
      : Banner(std::move(Banner)),
        GetExitCode([=](const Error &) { return DefaultErrorExitCode; }) {}

  /// Set the banner string for any errors caught by operator().
  void setBanner(std::string Banner) { this->Banner = std::move(Banner); }

  /// Set the exit-code mapper function.
  void setExitCodeMapper(std::function<int(const Error &)> GetExitCode) {
    this->GetExitCode = std::move(GetExitCode);
  }

  /// Check Err. If it's in a failure state log the error(s) and exit.
  void operator()(Error Err) const { checkError(std::move(Err)); }

  /// Check E. If it's in a success state then return the contained value. If
  /// it's in a failure state log the error(s) and exit.
  template <typename T> T operator()(Expected<T> &&E) const {
    checkError(E.takeError());
    return std::move(*E);
  }

  /// Check E. If it's in a success state then return the contained reference.
  /// If
  /// it's in a failure state log the error(s) and exit.
  template <typename T> T &operator()(Expected<T &> &&E) const {
    checkError(E.takeError());
    return *E;
  }

private:
  void checkError(Error Err) const {
    if (Err) {
      int ExitCode = GetExitCode(Err);
      logAllUnhandledErrors(std::move(Err), std::cerr, Banner);
      exit(ExitCode);
    }
  }

  std::string Banner;
  std::function<int(const Error &)> GetExitCode;
};

/// Helper for Expected<T>s used as out-parameters.
///
/// See ErrorAsOutParameter.
template <typename T> class ExpectedAsOutParameter {
public:
  ExpectedAsOutParameter(Expected<T> *ValOrErr) : ValOrErr(ValOrErr) {
    if (ValOrErr)
      (void)!!*ValOrErr;
  }

  ~ExpectedAsOutParameter() {
    if (ValOrErr)
      ValOrErr->setUnchecked();
  }

private:
  Expected<T> *ValOrErr;
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
inline void cantFail(Error Err) {
  if (Err)
    expected_unreachable("Failure value returned from cantFail wrapped call");
}

/// Report a fatal error if ValOrErr is a failure value, otherwise unwraps and
/// returns the contained value.
///
/// This function can be used to wrap calls to fallible functions ONLY when it
/// is known that the Error will always be a success value. E.g.
///
///   @code{.cpp}
///   // foo only attempts the fallible operation if DoFallibleOperation is
///   // true. If DoFallibleOperation is false then foo always returns an int.
///   Expected<int> foo(bool DoFallibleOperation);
///
///   int X = cantFail(foo(false));
///   @endcode
template <typename T> T cantFail(Expected<T> ValOrErr) {
  if (ValOrErr)
    return std::move(*ValOrErr);
  else
    expected_unreachable("Failure value returned from cantFail wrapped call");
}

/// Report a fatal error if ValOrErr is a failure value, otherwise unwraps and
/// returns the contained reference.
///
/// This function can be used to wrap calls to fallible functions ONLY when it
/// is known that the Error will always be a success value. E.g.
///
///   @code{.cpp}
///   // foo only attempts the fallible operation if DoFallibleOperation is
///   // true. If DoFallibleOperation is false then foo always returns a Bar&.
///   Expected<Bar&> foo(bool DoFallibleOperation);
///
///   Bar &X = cantFail(foo(false));
///   @endcode
template <typename T> T &cantFail(Expected<T &> ValOrErr) {
  if (ValOrErr)
    return *ValOrErr;
  else
    expected_unreachable("Failure value returned from cantFail wrapped call");
}

} // namespace llvm
