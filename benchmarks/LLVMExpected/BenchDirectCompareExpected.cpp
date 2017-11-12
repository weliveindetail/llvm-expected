 #include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

using namespace llvm;

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE Expected<int> DC_ExpectedSuccess_Fwd(int gt10) noexcept {
  return DC_ExpectedSuccess_Fwd<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE Expected<int> DC_ExpectedSuccess_Fwd<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10) // never happens
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode());

  benchmark::DoNotOptimize(gt10);
  return gt10;
}

template <int N>
void BM_DirectCompareExpected_ExpectedSuccess_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    auto res = DC_ExpectedSuccess_Fwd<N>(gt10);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE std::error_code
DC_ErrorCodeSuccess_Fwd(int gt10, int &res) noexcept {
  return DC_ErrorCodeSuccess_Fwd<N - 1>(gt10, res);
}

template<>
ATTRIBUTE_NOINLINE std::error_code
DC_ErrorCodeSuccess_Fwd<0>(int gt10, int &res) noexcept {
  if (fastrand() % 10 > gt10)
    return std::error_code(9, std::system_category());

  benchmark::DoNotOptimize(gt10);
  res = gt10;
  return std::error_code();
}

template <int N>
void BM_DirectCompareExpected_ErrorCodeSuccess_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccess_Fwd<N>(gt10, res)) {
      nulls << "[never happens]" << ec;
    }
  }
}

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE int DC_AdHocSuccess_Fwd(int gt10, int &res) noexcept {
  return DC_AdHocSuccess_Fwd<N - 1>(gt10, res);
}

template<>
ATTRIBUTE_NOINLINE int DC_AdHocSuccess_Fwd<0>(int gt10, int &res) noexcept {
  if (fastrand() % 10 > gt10)
    return 9;

  benchmark::DoNotOptimize(gt10);
  res = gt10;
  return 0;
}

template <int N>
void BM_DirectCompareExpected_AdHocSuccess_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;
    int gt10 = fastrand() % 10 + 100;
    if (int ec = DC_AdHocSuccess_Fwd<N>(gt10, res)) {
      nulls << "[never happens]" << ec;
    }
  }
}

// -----------------------------------------------------------------------------

BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ExpectedSuccess_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ExpectedSuccess_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ExpectedSuccess_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ExpectedSuccess_Fwd, 32);

BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd, 32);

BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_AdHocSuccess_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_AdHocSuccess_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_AdHocSuccess_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_DirectCompareExpected_AdHocSuccess_Fwd, 32);
