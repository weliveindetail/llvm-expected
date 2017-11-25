#include "common/Compiler.h"
#include "common/FastRand.h"

#include <outcome.hpp>
#include <benchmark/benchmark.h>
#include <sstream>
#include <string>

namespace outcome = OUTCOME_V2_NAMESPACE;

namespace PayloadOutcome {

class GlobPattern {
public:
  GlobPattern() noexcept {
    OutcomeGt10 = fastrand() % 10 + 100;
  }

  ATTRIBUTE_NOINLINE
  static outcome::result<GlobPattern, std::string>
  create(std::string input) noexcept {
    if (fastrand() % 10 > OutcomeGt10)
      return "invalid glob pattern: " + input; // never happens

    return GlobPattern();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int OutcomeGt10;
};

int GlobPattern::OutcomeGt10;

template <int N>
ATTRIBUTE_NOINLINE
outcome::result<bool, std::string> IMPL_PayloadOutcome() noexcept {
  return IMPL_PayloadOutcome<N - 1>();
}

template <>
ATTRIBUTE_NOINLINE
outcome::result<bool, std::string> IMPL_PayloadOutcome<0>() noexcept {
  std::string fileName = "[a*.txt";

  auto pattern = GlobPattern::create(std::move(fileName));
  if (!pattern)
    return pattern.error();

  return pattern.value().match("...");
}

template <int N>
void BM_PayloadOutcome(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    auto res = IMPL_PayloadOutcome<N>();

    if (!res) {
      nulls << "[OverheadExample] " << res.error();
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_PayloadOutcome, 0);
BENCHMARK_TEMPLATE1(BM_PayloadOutcome, 2);
BENCHMARK_TEMPLATE1(BM_PayloadOutcome, 8);
BENCHMARK_TEMPLATE1(BM_PayloadOutcome, 16);

}
