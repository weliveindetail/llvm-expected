#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <sstream>

namespace FwdIntAdHoc {

template <int N>
ATTRIBUTE_NOINLINE int IMPL_FwdIntAdHoc(int gt10, int &res) noexcept {
  gt10 = workload(gt10);

  if (int ec = IMPL_FwdIntAdHoc<N - 1>(gt10, res))
    return ec; // never happens

  res = workload(res);
  return 0;
}

template<>
ATTRIBUTE_NOINLINE int IMPL_FwdIntAdHoc<1>(int gt10, int &res) noexcept {
  if (workload(gt10) < 10)
    return 9; // never happens

  res = workload(gt10);
  return 0;
}

template <int N>
void BM_FwdIntAdHoc(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    int res;

    if (int ec = IMPL_FwdIntAdHoc<N>(gt10, res)) {
      nulls << "[never happens]" << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 1);
BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 4);
BENCHMARK_TEMPLATE1(BM_FwdIntAdHoc, 8);

}
