#include "FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>

class GlobPattern {
public:
  GlobPattern() noexcept {
    AdHocGt10 = fastrand() % 10 + 100;
  }

  ATTRIBUTE_NOINLINE
  static bool create(std::string input, GlobPattern &result) noexcept {
    if (fastrand() % 10 > AdHocGt10)
      return false; // never happens

    result = GlobPattern();
    return true;
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int AdHocGt10;
};

int GlobPattern::AdHocGt10;

template <int N>
ATTRIBUTE_NOINLINE bool IMPL_PayloadAdHoc(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  return IMPL_PayloadAdHoc<N - 1>(result, errorFileName);
}

template <>
ATTRIBUTE_NOINLINE bool IMPL_PayloadAdHoc<0>(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  GlobPattern pattern;
  std::string fileName = "[a*.txt";

  if (!GlobPattern::create(fileName, pattern)) {
    errorFileName = std::make_unique<std::string>(fileName);
    return false;
  }

  result = pattern.match("...");
  return true;
}

template <int N>
void BM_PayloadAdHoc(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    bool res;
    std::unique_ptr<std::string> errorFileName = nullptr;

    if (!IMPL_PayloadAdHoc<N>(res, errorFileName)) {
      nulls << "[OverheadExample] " << *errorFileName;
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_PayloadAdHoc, 0);
BENCHMARK_TEMPLATE1(BM_PayloadAdHoc, 2);
BENCHMARK_TEMPLATE1(BM_PayloadAdHoc, 8);
BENCHMARK_TEMPLATE1(BM_PayloadAdHoc, 16);
