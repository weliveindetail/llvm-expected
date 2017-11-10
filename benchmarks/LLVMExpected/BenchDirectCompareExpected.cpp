 #include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

using namespace llvm;

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
Expected<int> DC_ExpectedSuccess(int gt10) noexcept {
  if (fastrand() % 10 > gt10) // never happens
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode());

  return gt10;
}

ATTRIBUTE_NOINLINE
Expected<int> DC_ExpectedSuccessFwd1(int gt10) noexcept {
  return DC_ExpectedSuccess(gt10);
}

ATTRIBUTE_NOINLINE
Expected<int> DC_ExpectedSuccessFwd2(int gt10) noexcept {
  return DC_ExpectedSuccessFwd1(gt10);
}

ATTRIBUTE_NOINLINE
Expected<int> DC_ExpectedSuccessFwd3(int gt10) noexcept {
  return DC_ExpectedSuccessFwd2(gt10);
}

void BM_DirectCompareExpected_ExpectedSuccess_Fwd0(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    auto res = DC_ExpectedSuccess(gt10);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }

    benchmark::DoNotOptimize(res);
  }
}

void BM_DirectCompareExpected_ExpectedSuccess_Fwd1(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    auto res = DC_ExpectedSuccessFwd1(gt10);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }

    benchmark::DoNotOptimize(res);
  }
}

void BM_DirectCompareExpected_ExpectedSuccess_Fwd2(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    auto res = DC_ExpectedSuccessFwd2(gt10);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }

    benchmark::DoNotOptimize(res);
  }
}

void BM_DirectCompareExpected_ExpectedSuccess_Fwd3(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    auto res = DC_ExpectedSuccessFwd3(gt10);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccess(int gt10, int &res) noexcept {
  if (fastrand() % 10 > gt10)
    return std::error_code(9, std::system_category());

  res = gt10;
  return std::error_code();
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd0(int gt10, int &res) noexcept {
  return DC_ErrorCodeSuccess(gt10, res);
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd1(int gt10, int &res) noexcept {
  return DC_ErrorCodeSuccessFwd0(gt10, res);
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd2(int gt10, int &res) noexcept {
  return DC_ErrorCodeSuccessFwd1(gt10, res);
}

ATTRIBUTE_NOINLINE
std::error_code DC_ErrorCodeSuccessFwd3(int gt10, int &res) noexcept {
  return DC_ErrorCodeSuccessFwd2(gt10, res);
}

void BM_DirectCompareExpected_ErrorCodeSuccess_Fwd0(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccess(gt10, res)) {
      nulls << "[never happens]" << ec;
    }
  }
}

void BM_DirectCompareExpected_ErrorCodeSuccess_Fwd1(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccessFwd1(gt10, res)) {
      nulls << "[never happens]" << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

void BM_DirectCompareExpected_ErrorCodeSuccess_Fwd2(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccessFwd2(gt10, res)) {
      nulls << "[never happens]" << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

void BM_DirectCompareExpected_ErrorCodeSuccess_Fwd3(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = DC_ErrorCodeSuccessFwd3(gt10, res)) {
      nulls << "[never happens]" << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_DirectCompareExpected_ExpectedSuccess_Fwd0);
BENCHMARK(BM_DirectCompareExpected_ExpectedSuccess_Fwd1);
BENCHMARK(BM_DirectCompareExpected_ExpectedSuccess_Fwd2);
BENCHMARK(BM_DirectCompareExpected_ExpectedSuccess_Fwd3);

BENCHMARK(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd0);
BENCHMARK(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd1);
BENCHMARK(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd2);
BENCHMARK(BM_DirectCompareExpected_ErrorCodeSuccess_Fwd3);
