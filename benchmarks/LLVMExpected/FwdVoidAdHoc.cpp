#include "FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>

namespace FwdVoidAdHoc {

template <int N>
ATTRIBUTE_NOINLINE int IMPL_FwdVoidAdHoc(int gt10) noexcept {
  return IMPL_FwdVoidAdHoc<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE int IMPL_FwdVoidAdHoc<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10)
    return 9;

  return 0;
}

template <int N>
void BM_FwdVoidAdHoc(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    if (int ec = IMPL_FwdVoidAdHoc<N>(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 0);
BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 8);
BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 16);

}
