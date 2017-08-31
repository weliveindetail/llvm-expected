#include "BenchSuccessRates.h"
#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <system_error>

__attribute__((noinline))
static std::error_code SuccessAlways_ErrorCode(int input, int &res) noexcept {
  if (input % 3 == 0) { // 3, 6, 9, 12, ...
    res = input + 2;
    return std::error_code();
  }

  if (input % 6 != 0) { // 1, 2, 4, 5, 7, 8, 10, 11, ...
    res = input - 3;
    return std::error_code();
  }

  return std::error_code(9, std::system_category()); // never
}

void BM_Rate_SuccessAlways_ErrorCode(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    auto ec = SuccessAlways_ErrorCode(fastrand(), res);

    benchmark::DoNotOptimize(ec);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static std::error_code Success2outof3_ErrorCode(int input, int &res) noexcept {
  if (input % 3 == 0) { // 3, 6, 9, 12, ...
    res = input + 2;
    return std::error_code();
  }

  if (input % 2 == 0) { // 2, 4, 8, 10, ...
    res = input - 3;
    return std::error_code();
  }

  return std::error_code(9, std::system_category()); // 1, 5, 7, 11, ...
}

void BM_Rate_Success2outOf3_ErrorCode(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    auto ec = Success2outof3_ErrorCode(fastrand(), res);

    benchmark::DoNotOptimize(ec);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static std::error_code Success1outof3_ErrorCode(int input, int &res) noexcept {
  if (input % 6 == 0) { // 6, ...
    res = input + 2;
    return std::error_code();
  }

  if (input % 3 == 0) { // 3, 9, ...
    res = input - 3;
    return std::error_code();
  }

  return std::error_code(9, std::system_category()); // 1, 2, 4, 5, 7, 8 ...
}

void BM_Rate_Success1outOf3_ErrorCode(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    auto ec = Success1outof3_ErrorCode(fastrand(), res);

    benchmark::DoNotOptimize(ec);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static std::error_code SuccessNever_ErrorCode(int input, int &res) noexcept {
  if (input % 3 == 3) { // never
    res = input + 2;
    return std::error_code();
  }

  if (input % 6 > 5) { // never
    res = input - 3;
    return std::error_code();
  }

  return std::error_code(9, std::system_category()); // always
}

void BM_Rate_SuccessNever_ErrorCode(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    auto ec = SuccessNever_ErrorCode(fastrand(), res);

    benchmark::DoNotOptimize(ec);
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

__attribute__((noinline))
static llvm::Expected<int> SuccessAlways_Expected(int input) noexcept {
  if (input % 3 == 0) // 3, 6, 9, 12, ...
    return input + 2;

  if (input % 6 != 0) // 1, 2, 4, 5, 7, 8, 10, 11, ...
    return input - 3;

  return llvm::make_error<llvm::StringError>( // never
      "Mocked Error", llvm::inconvertibleErrorCode());
}

void BM_Rate_SuccessAlways_Expected(benchmark::State &state) {
  while (state.KeepRunning()) {
    auto res = SuccessAlways_Expected(fastrand());

#ifdef NDEBUG
    benchmark::DoNotOptimize(res);
#else
    llvm::consumeError(res.takeError());
#endif
  }
}

__attribute__((noinline))
static llvm::Expected<int> Success2outof3_Expected(int input) noexcept {
  if (input % 3 == 0) // 3, 6, 9, 12, ...
    return input + 2;

  if (input % 2 == 0) // 2, 4, 8, 10, ...
    return input - 3;

  return llvm::make_error<llvm::StringError>( // 1, 5, 7, 11, ...
      "Mocked Error", llvm::inconvertibleErrorCode());
}

void BM_Rate_Success2outOf3_Expected(benchmark::State &state) {
  while (state.KeepRunning()) {
    auto res = Success2outof3_Expected(fastrand());

#ifdef NDEBUG
    benchmark::DoNotOptimize(res);
#else
    llvm::consumeError(res.takeError());
#endif
  }
}

__attribute__((noinline))
static llvm::Expected<int> Success1outof3_Expected(int input) noexcept {
  if (input % 6 == 0) // 6, ...
    return input + 2;

  if (input % 3 == 0) // 3, 9, ...
    return input - 3;

  return llvm::make_error<llvm::StringError>( // 1, 2, 4, 5, 7, 8 ...
      "Mocked Error", llvm::inconvertibleErrorCode());
}

void BM_Rate_Success1outOf3_Expected(benchmark::State &state) {
  while (state.KeepRunning()) {
    auto res = Success1outof3_Expected(fastrand());

#ifdef NDEBUG
    benchmark::DoNotOptimize(res);
#else
    llvm::consumeError(res.takeError());
#endif
  }
}

__attribute__((noinline))
static llvm::Expected<int> SuccessNever_Expected(int input) noexcept {
  if (input % 3 == 3) // never
    return input + 2;

  if (input % 6 > 5) // never
    return input - 3;

  return llvm::make_error<llvm::StringError>( // always
      "Mocked Error", llvm::inconvertibleErrorCode());
}

void BM_Rate_SuccessNever_Expected(benchmark::State &state) {
  while (state.KeepRunning()) {
    auto res = SuccessNever_Expected(fastrand());

#ifdef NDEBUG
    benchmark::DoNotOptimize(res);
#else
    llvm::consumeError(res.takeError());
#endif
  }
}
