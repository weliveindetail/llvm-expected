#include "FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <system_error>

template <int N>
ATTRIBUTE_NOINLINE std::error_code IMPL_FwdVoidErrorCode(int gt10) noexcept {
  return IMPL_FwdVoidErrorCode<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE std::error_code IMPL_FwdVoidErrorCode<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10)
    return std::error_code(9, std::system_category());

  return std::error_code();
}

template <int N>
void BM_FwdVoidErrorCode(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (std::error_code ec = IMPL_FwdVoidErrorCode<N>(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 0);
BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 8);
BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 16);
