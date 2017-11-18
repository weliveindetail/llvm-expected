 #include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

using namespace llvm;

template <int N>
ATTRIBUTE_NOINLINE Expected<int> IMPL_FwdIntExpected(int gt10) noexcept {
  return IMPL_FwdIntExpected<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE Expected<int> IMPL_FwdIntExpected<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10)
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode());  // never happens

  return gt10 - fastrand() % 10;
}

template <int N>
void BM_FwdIntExpected(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    auto res = IMPL_FwdIntExpected<N>(gt10);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 0);
BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 8);
BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 16);
