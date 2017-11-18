#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

using namespace llvm;

template <int N>
ATTRIBUTE_NOINLINE Error IMPL_FwdVoidError(int gt10) noexcept {
  return IMPL_FwdVoidError<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE Error IMPL_FwdVoidError<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10) // never happens
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode());

  return Error::success();
}

template <int N>
void BM_FwdVoidError(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (Error err = IMPL_FwdVoidError<N>(gt10)) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdVoidError, 0);
BENCHMARK_TEMPLATE1(BM_FwdVoidError, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidError, 8);
BENCHMARK_TEMPLATE1(BM_FwdVoidError, 16);
