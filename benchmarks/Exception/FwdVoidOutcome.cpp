#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <outcome/outcome.hpp>
#include <benchmark/benchmark.h>
#include <string>
#include <sstream>

namespace outcome = OUTCOME_V2_NAMESPACE;

namespace FwdVoidOutcome {

template <int N>
ATTRIBUTE_NOINLINE outcome::result<void, std::string>
IMPL_FwdVoidOutcome(int gt10) noexcept {
  workload(gt10);

  auto ret = IMPL_FwdVoidOutcome<N - 1>(gt10);
  if (!ret)
    return ret.error(); // never happens

  workload(0);
  return outcome::success();
}

template <>
ATTRIBUTE_NOINLINE outcome::result<void, std::string>
IMPL_FwdVoidOutcome<1>(int gt10) noexcept {
  if (workload(gt10) < 10)
    return "Mocked Error"; // never happens

  workload(0);
  return outcome::success();
}

template <int N>
void BM_FwdVoidOutcome(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    auto ret = IMPL_FwdVoidOutcome<N>(gt10);

    if (!ret) {
      nulls << "[never happens]" << ret.error();
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdVoidOutcome, 1);
BENCHMARK_TEMPLATE1(BM_FwdVoidOutcome, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidOutcome, 4);
BENCHMARK_TEMPLATE1(BM_FwdVoidOutcome, 8);

}
