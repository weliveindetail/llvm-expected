#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <stdexcept>

namespace FwdIntException {

template <int N>
ATTRIBUTE_NOINLINE int IMPL_FwdIntException(int gt10) {
  workload(gt10);
  int res = IMPL_FwdIntException<N - 1>(gt10);
  return workload(res);
}

template <>
ATTRIBUTE_NOINLINE int IMPL_FwdIntException<1>(int gt10) {
  if (workload(gt10) < 10)
    throw std::runtime_error("Error Message"); // never happens

  return workload(gt10);
}

template <int N>
void BM_FwdIntException(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    int res;

    try {
      res = IMPL_FwdIntException<N>(gt10);
    } catch (std::runtime_error e) {
      nulls << "[never happens] " << e.what() << "\n";
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK_TEMPLATE1(BM_FwdIntException, 1);
BENCHMARK_TEMPLATE1(BM_FwdIntException, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntException, 4);
BENCHMARK_TEMPLATE1(BM_FwdIntException, 8);

}
