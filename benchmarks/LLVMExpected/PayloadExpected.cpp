#include "common/Compiler.h"
#include "common/FastRand.h"

#include <Errors.h>
#include <Expected.h>
#include <benchmark/benchmark.h>

using namespace llvm;

namespace PayloadExpected {

class GlobPattern {
public:
  GlobPattern() noexcept {
    ExpectedGt10 = fastrand() % 10 + 100;
  }

  ATTRIBUTE_NOINLINE
  static Expected<GlobPattern> create(std::string input) noexcept {
    if (fastrand() % 10 > ExpectedGt10)
      return make_error<StringError>(
          "invalid glob pattern: " + input,
          std::make_error_code(std::errc::invalid_argument)); // never happens

    return GlobPattern();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int ExpectedGt10;
};

int GlobPattern::ExpectedGt10;

template <int N>
ATTRIBUTE_NOINLINE Expected<bool> IMPL_PayloadExpected() noexcept {
  return IMPL_PayloadExpected<N - 1>();
}

template <>
ATTRIBUTE_NOINLINE Expected<bool> IMPL_PayloadExpected<0>() noexcept {
  std::string fileName = "[a*.txt";

  Expected<GlobPattern> pattern = GlobPattern::create(std::move(fileName));
  if (!pattern)
    return pattern.takeError();

  return pattern->match("...");
}

template <int N>
void BM_PayloadExpected(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    Expected<bool> res = IMPL_PayloadExpected<N>();

    if (auto err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[OverheadExample] ");
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_PayloadExpected, 0);
BENCHMARK_TEMPLATE1(BM_PayloadExpected, 2);
BENCHMARK_TEMPLATE1(BM_PayloadExpected, 8);
BENCHMARK_TEMPLATE1(BM_PayloadExpected, 16);

}
