#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <system_error>

namespace FwdVoidErrorCode {

template <int N>
ATTRIBUTE_NOINLINE std::error_code IMPL_FwdVoidErrorCode(int gt10) noexcept {
  gt10 = workload(gt10);

  if (std::error_code ec = IMPL_FwdVoidErrorCode<N - 1>(gt10))
    return ec; // never happens

  workload(0);
  return std::error_code();
}

template <>
ATTRIBUTE_NOINLINE std::error_code IMPL_FwdVoidErrorCode<1>(int gt10) noexcept {
  if (workload(gt10) < 10)
    return std::error_code(9, std::system_category()); // never happens

  workload(0);
  return std::error_code();
}

template <int N>
void BM_FwdVoidErrorCode(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    if (std::error_code ec = IMPL_FwdVoidErrorCode<N>(gt10)) {
      nulls << "[never happens]" << ec;
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 1);
BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 4);
BENCHMARK_TEMPLATE1(BM_FwdVoidErrorCode, 8);

}
