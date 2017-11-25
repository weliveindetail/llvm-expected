#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/outcome.hpp"
#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <string>

#if ENABLE_BENCHMARK_OUTCOME

namespace outcome = OUTCOME_V2_NAMESPACE;

namespace FwdIntOutcome {

template <int N>
ATTRIBUTE_NOINLINE outcome::result<int, std::string>
IMPL_FwdIntOutcome(int gt10) noexcept {
  workload(gt10);

  auto res = IMPL_FwdIntOutcome<N - 1>(gt10);
  if (!res)
    return res.error(); // never happens

  return workload(res.value());
}

template <>
ATTRIBUTE_NOINLINE outcome::result<int, std::string>
IMPL_FwdIntOutcome<1>(int gt10) noexcept {
  if (workload(gt10) < 10)
    return "Error Message";  // never happens

  return workload(gt10);
}

template <int N>
void BM_FwdIntOutcome(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    auto res = IMPL_FwdIntOutcome<N>(gt10);

    if (!res) {
      nulls << "[never happens]" << res.error();
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 1);
BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 4);
BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 8);

}

#endif
