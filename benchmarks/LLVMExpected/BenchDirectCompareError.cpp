#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

using namespace llvm;

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE Error DC_ErrorSuccess_Fwd(int gt10) noexcept {
  benchmark::DoNotOptimize(gt10);
  return DC_ErrorSuccess_Fwd<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE Error DC_ErrorSuccess_Fwd<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10) // never happens
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode());

  return Error::success();
}

template <int N>
void BM_DirectCompareError_ErrorSuccess_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (Error err = DC_ErrorSuccess_Fwd<N>(gt10)) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE std::error_code
DC_ErrorCodeSuccess_Fwd(int gt10) noexcept {
  benchmark::DoNotOptimize(gt10);
  return DC_ErrorCodeSuccess_Fwd<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE std::error_code
DC_ErrorCodeSuccess_Fwd<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10)
    return std::error_code(9, std::system_category());

  return std::error_code();
}

template <int N>
void BM_DirectCompareError_ErrorCodeSuccess_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccess_Fwd<N>(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE int DC_AdHocSuccess_Fwd(int gt10) noexcept {
  benchmark::DoNotOptimize(gt10);
  return DC_AdHocSuccess_Fwd<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE int DC_AdHocSuccess_Fwd<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10)
    return 9;

  return 0;
}

template <int N>
void BM_DirectCompareError_AdHocSuccess_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (int ec = DC_AdHocSuccess_Fwd<N>(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

// -----------------------------------------------------------------------------

BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorSuccess_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorSuccess_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorSuccess_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorSuccess_Fwd, 32);

BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorCodeSuccess_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorCodeSuccess_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorCodeSuccess_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_ErrorCodeSuccess_Fwd, 32);

BENCHMARK_TEMPLATE1(BM_DirectCompareError_AdHocSuccess_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_AdHocSuccess_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_AdHocSuccess_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_DirectCompareError_AdHocSuccess_Fwd, 32);
