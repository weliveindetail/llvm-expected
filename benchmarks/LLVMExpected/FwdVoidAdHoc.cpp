#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <sstream>

namespace FwdVoidAdHoc {

template <int N>
ATTRIBUTE_NOINLINE int IMPL_FwdVoidAdHoc(int gt10) noexcept {
  gt10 = workload(gt10);

  if (int ec = IMPL_FwdVoidAdHoc<N - 1>(gt10))
    return ec; // never happens

  workload(0);
  return 0;
}

template <>
ATTRIBUTE_NOINLINE int IMPL_FwdVoidAdHoc<1>(int gt10) noexcept {
  if (workload(gt10) < 10)
    return 9; // never happens

  workload(0);
  return 0;
}

template <int N>
void BM_FwdVoidAdHoc(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    if (int ec = IMPL_FwdVoidAdHoc<N>(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 1);
BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 4);
BENCHMARK_TEMPLATE1(BM_FwdVoidAdHoc, 8);

}
