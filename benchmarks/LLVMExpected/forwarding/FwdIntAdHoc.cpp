 #include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

using namespace llvm;

template <int N>
ATTRIBUTE_NOINLINE int IMPL_FwdIntAdHoc(int gt10, int &res) noexcept {
  return IMPL_FwdIntAdHoc<N - 1>(gt10, res);
}

template<>
ATTRIBUTE_NOINLINE int IMPL_FwdIntAdHoc<0>(int gt10, int &res) noexcept {
  if (fastrand() % 10 > gt10)
    return 9; // never happens

  res = gt10 - fastrand() % 10;
  return 0;
}

template <int N>
void BM_FwdIntAdHoc(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;
    int gt10 = fastrand() % 10 + 100;

    if (int ec = IMPL_FwdIntAdHoc<N>(gt10, res)) {
      nulls << "[never happens]" << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 0);
BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 8);
BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 16);
