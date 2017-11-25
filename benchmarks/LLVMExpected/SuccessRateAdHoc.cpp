#include "common/Compiler.h"
#include "common/FastRand.h"

#include <benchmark/benchmark.h>
#include <sstream>

namespace SuccessRateAdHoc {

ATTRIBUTE_NOINLINE
int IMPL_SuccessRateAdHoc(int successRate, int &res) noexcept {
  if (fastrand() % 100 > successRate)
    return 9;

  res = successRate - fastrand() % 10;
  return 0;
}

void BM_SuccessRateAdHoc(benchmark::State &state) {
  int successRate = state.range(0);
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    int res;

    if (int ec = IMPL_SuccessRateAdHoc(successRate, res)) {
      nulls << "[OverheadExample] " << ec;
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK(BM_SuccessRateAdHoc)->Arg(100)->Arg(95)->Arg(50)->Arg(0);

}
