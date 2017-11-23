#include "FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <system_error>

namespace SuccessRateErrorCode {

ATTRIBUTE_NOINLINE
std::error_code IMPL_SuccessRateErrorCode(int successRate, int &res) noexcept {
  if (fastrand() % 100 > successRate)
    return std::make_error_code(std::errc::invalid_argument);

  res = successRate - fastrand() % 10;
  return std::error_code();
}

void BM_SuccessRateErrorCode(benchmark::State &state) {
  int successRate = state.range(0);
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;

    if (std::error_code ec = IMPL_SuccessRateErrorCode(successRate, res)) {
      nulls << "[OverheadExample] " << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK(BM_SuccessRateErrorCode)->Arg(100)->Arg(95)->Arg(50)->Arg(0);

}
