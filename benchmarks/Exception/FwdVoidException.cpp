#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <stdexcept>

namespace FwdVoidException {

template <int N>
ATTRIBUTE_NOINLINE void IMPL_FwdVoidException(int gt10) {
  workload(gt10);
  IMPL_FwdVoidException<N - 1>(gt10);
  workload(0);
}

template <>
ATTRIBUTE_NOINLINE void IMPL_FwdVoidException<1>(int gt10) {
  if (workload(gt10) < 10)
    throw std::runtime_error("Error Message"); // never happens

  workload(0);
}

template <int N>
void BM_FwdVoidException(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    try {
      IMPL_FwdVoidException<N>(gt10);
    } catch (std::runtime_error e) {
      nulls << "[never happens]" << e.what() << "\n";
    }
  }
}

// -----------------------------------------------------------------------------

BENCHMARK_TEMPLATE1(BM_FwdVoidException, 1);
BENCHMARK_TEMPLATE1(BM_FwdVoidException, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidException, 4);
BENCHMARK_TEMPLATE1(BM_FwdVoidException, 8);

}
