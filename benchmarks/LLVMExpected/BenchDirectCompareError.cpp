#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

using namespace llvm;

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
Error DC_ErrorSuccess(int gt10) noexcept {
  if (fastrand() % 10 > gt10) // never happens
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode());

  return Error::success();
}

ATTRIBUTE_NOINLINE
Error DC_ErrorSuccessFwd1(int gt10) noexcept {
  return DC_ErrorSuccess(gt10);
}

ATTRIBUTE_NOINLINE
Error DC_ErrorSuccessFwd2(int gt10) noexcept {
  return DC_ErrorSuccessFwd1(gt10);
}

ATTRIBUTE_NOINLINE
Error DC_ErrorSuccessFwd3(int gt10) noexcept {
  return DC_ErrorSuccessFwd2(gt10);
}

void BM_DirectCompareError_ErrorSuccess_Fwd0(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (Error err = DC_ErrorSuccess(gt10)) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

void BM_DirectCompareError_ErrorSuccess_Fwd1(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (Error err = DC_ErrorSuccessFwd1(gt10)) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

void BM_DirectCompareError_ErrorSuccess_Fwd2(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (Error err = DC_ErrorSuccessFwd2(gt10)) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

void BM_DirectCompareError_ErrorSuccess_Fwd3(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (Error err = DC_ErrorSuccessFwd3(gt10)) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccess(int gt10) noexcept {
  if (fastrand() % 10 > gt10)
    return std::error_code(9, std::system_category());

  return std::error_code();
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd0(int gt10) noexcept {
  return DC_ErrorCodeSuccess(gt10);
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd1(int gt10) noexcept {
  return DC_ErrorCodeSuccessFwd0(gt10);
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd2(int gt10) noexcept {
  return DC_ErrorCodeSuccessFwd1(gt10);
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd3(int gt10) noexcept {
  return DC_ErrorCodeSuccessFwd2(gt10);
}

void BM_DirectCompareError_ErrorCodeSuccess_Fwd0(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccess(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

void BM_DirectCompareError_ErrorCodeSuccess_Fwd1(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccessFwd1(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

void BM_DirectCompareError_ErrorCodeSuccess_Fwd2(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccessFwd2(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

void BM_DirectCompareError_ErrorCodeSuccess_Fwd3(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccessFwd3(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_DirectCompareError_ErrorSuccess_Fwd0);
BENCHMARK(BM_DirectCompareError_ErrorSuccess_Fwd1);
BENCHMARK(BM_DirectCompareError_ErrorSuccess_Fwd2);
BENCHMARK(BM_DirectCompareError_ErrorSuccess_Fwd3);
BENCHMARK(BM_DirectCompareError_ErrorCodeSuccess_Fwd0);
BENCHMARK(BM_DirectCompareError_ErrorCodeSuccess_Fwd1);
BENCHMARK(BM_DirectCompareError_ErrorCodeSuccess_Fwd2);
BENCHMARK(BM_DirectCompareError_ErrorCodeSuccess_Fwd3);
