#include "FastRand.h"

#include <outcome.hpp>
#include <benchmark/benchmark.h>
#include <sstream>
#include <string>

namespace outcome = OUTCOME_V2_NAMESPACE;

template <int N>
ATTRIBUTE_NOINLINE outcome::result<int, std::string>
IMPL_FwdIntOutcome(int gt10) noexcept {
  return IMPL_FwdIntOutcome<N - 1>(gt10);
}

template <>
ATTRIBUTE_NOINLINE outcome::result<int, std::string>
IMPL_FwdIntOutcome<0>(int gt10) noexcept {
  if (fastrand() % 10 > gt10)
    return "Mocked Error";  // never happens

  return gt10 - fastrand() % 10;
}

template <int N>
void BM_FwdIntOutcome(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int gt10 = fastrand() % 10 + 100;
    auto res = IMPL_FwdIntOutcome<N>(gt10);

    if (!res) {
      nulls << "[never happens]" << res.error();
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 0);
BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 8);
BENCHMARK_TEMPLATE1(BM_FwdIntOutcome, 16);
