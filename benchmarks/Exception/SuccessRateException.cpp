#include "FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>
#include <stdexcept>

namespace SuccessRateException {

ATTRIBUTE_NOINLINE
static int IMPL_SuccessRateException(int successRate) {
  if (fastrand() % 100 > successRate)
    throw std::runtime_error("Mocked Error");

  return successRate;
}

void BM_SuccessRateException(benchmark::State &state) {
  std::ostringstream nulls;
  int successRate = state.range(0);

  while (state.KeepRunning()) {
    int res;

    try {
      res = IMPL_SuccessRateException(successRate);
    } catch (std::runtime_error e) {
      nulls << "[OverheadExample] " << e.what() << "\n";
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK(BM_SuccessRateException)->Arg(100)->Arg(95)->Arg(50)->Arg(0);

}
