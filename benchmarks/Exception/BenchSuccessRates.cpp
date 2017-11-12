#include "FastRand.h"

#include <benchmark/benchmark.h>

#include <sstream>
#include <stdexcept>

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
static int Minimal_ThrowInt(int successRate) {
  if (fastrand() % 100 > successRate)
    throw successRate;

  return successRate;
}

void BM_SuccessRate_Minimal_ThrowInt(benchmark::State &state) {
  std::ostringstream nulls;
  int successRate = state.range(0);

  while (state.KeepRunning()) {
    int res;
    try {
      res = Minimal_ThrowInt(successRate);
    } catch (int e) {
      nulls << "[OverheadExample] " << e << "\n";
    }
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
static int Minimal_ThrowException(int successRate) {
  if (fastrand() % 100 > successRate)
    throw std::runtime_error("Mocked Error");

  return successRate;
}

void BM_SuccessRate_Minimal_ThrowException(benchmark::State &state) {
  std::ostringstream nulls;
  int successRate = state.range(0);

  while (state.KeepRunning()) {
    int res;
    try {
      res = Minimal_ThrowException(successRate);
    } catch (std::runtime_error e) {
      nulls << "[OverheadExample] " << e.what() << "\n";
    }
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_SuccessRate_Minimal_ThrowInt)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
BENCHMARK(BM_SuccessRate_Minimal_ThrowException)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
