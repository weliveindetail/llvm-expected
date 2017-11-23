#include "FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <system_error>

class GlobPattern {
public:
  GlobPattern() noexcept {
    ErrorCodeGt10 = fastrand() % 10 + 100;
  }

  ATTRIBUTE_NOINLINE
  static std::error_code
  create(std::string input, GlobPattern &result) noexcept {
    if (fastrand() % 10 > ErrorCodeGt10)
      return std::make_error_code(std::errc::invalid_argument); // never happens

    result = GlobPattern();
    return std::error_code();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int ErrorCodeGt10;
};

int GlobPattern::ErrorCodeGt10;

template <int N>
ATTRIBUTE_NOINLINE
std::error_code IMPL_PayloadErrorCode(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  return IMPL_PayloadErrorCode<N - 1>(result, errorFileName);
}

template <>
ATTRIBUTE_NOINLINE
std::error_code IMPL_PayloadErrorCode<0>(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  GlobPattern pattern;
  std::string fileName = "[a*.txt";

  if (std::error_code ec = GlobPattern::create(fileName, pattern)) {
    errorFileName = std::make_unique<std::string>(fileName);
    return ec;
  }

  result = pattern.match("...");
  return std::error_code();
}

template <int N>
void BM_PayloadErrorCode(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    bool res;
    std::unique_ptr<std::string> errorFileName = nullptr;

    if (std::error_code ec = IMPL_PayloadErrorCode<N>(res, errorFileName)) {
      nulls << "[OverheadExample] " << ec.value() << ": ";
      nulls << *errorFileName << "\n";
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_PayloadErrorCode, 0);
BENCHMARK_TEMPLATE1(BM_PayloadErrorCode, 2);
BENCHMARK_TEMPLATE1(BM_PayloadErrorCode, 8);
BENCHMARK_TEMPLATE1(BM_PayloadErrorCode, 16);
