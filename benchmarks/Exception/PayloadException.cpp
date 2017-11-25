#include "common/Compiler.h"
#include "common/FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <stdexcept>

namespace PayloadException {

class GlobPattern {
public:
  GlobPattern() noexcept {
    Gt10 = fastrand() % 10 + 100;
  }

  ATTRIBUTE_NOINLINE
  static GlobPattern create(std::string input) {
    if (fastrand() % 10 > Gt10)
      throw std::runtime_error("invalid glob pattern: " + input);

    return GlobPattern();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int Gt10;
};

int GlobPattern::Gt10;

template <int N>
ATTRIBUTE_NOINLINE
bool IMPL_PayloadException() {
  return IMPL_PayloadException<N - 1>();
}

template <>
ATTRIBUTE_NOINLINE
bool IMPL_PayloadException<1>() {
  std::string fileName = "[a*.txt";

  GlobPattern pattern = GlobPattern::create(std::move(fileName));
  return pattern.match("...");
}

template <int N>
void BM_PayloadException(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    bool res;

    try {
      res = IMPL_PayloadException<N>();
    } catch (std::runtime_error e) {
      nulls << "[OverheadExample] " << e.what() << "\n";
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_PayloadException, 1);
BENCHMARK_TEMPLATE1(BM_PayloadException, 4);
BENCHMARK_TEMPLATE1(BM_PayloadException, 8);
BENCHMARK_TEMPLATE1(BM_PayloadException, 32);

}
