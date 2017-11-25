#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <system_error>

namespace FwdIntErrorCode {

template <int N>
ATTRIBUTE_NOINLINE std::error_code
IMPL_FwdIntErrorCode(int gt10, int &res) noexcept {
  gt10 = workload(gt10);

  if (std::error_code ec = IMPL_FwdIntErrorCode<N - 1>(gt10, res))
    return ec; // never happens

  res = workload(res);
  return std::error_code();
}

template<>
ATTRIBUTE_NOINLINE std::error_code
IMPL_FwdIntErrorCode<1>(int gt10, int &res) noexcept {
  if (workload(gt10) < 10)
    return std::error_code(9, std::system_category()); // never happens

  res = workload(gt10);
  return std::error_code();
}

template <int N>
void BM_FwdIntErrorCode(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    int res;

    if (std::error_code ec = IMPL_FwdIntErrorCode<N>(gt10, res)) {
      nulls << "[never happens]" << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_FwdIntErrorCode, 1);
BENCHMARK_TEMPLATE1(BM_FwdIntErrorCode, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntErrorCode, 4);
BENCHMARK_TEMPLATE1(BM_FwdIntErrorCode, 8);

}
