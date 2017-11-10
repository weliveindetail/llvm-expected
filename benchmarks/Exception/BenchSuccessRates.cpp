#include "FastRand.h"

#include <benchmark/benchmark.h>

#include <stdexcept>

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
int Minimal_ThrowInt(int successRate) {
  if (fastrand() % 100 > successRate)
    throw successRate;

  return successRate;
}

void BM_SuccessRate_Minimal_ThrowInt(benchmark::State &state) {
  int successRate = state.range(0);

  while (state.KeepRunning()) {
    int res;
    int err;
    try {
      res = Minimal_ThrowInt(successRate);
    } catch (int e) {
      err = e;
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
int Minimal_ThrowException(int successRate) {
  if (fastrand() % 100 > successRate)
    throw std::runtime_error("Mocked Error");

  return successRate;
}

void BM_SuccessRate_Minimal_ThrowException(benchmark::State &state) {
  int successRate = state.range(0);

  while (state.KeepRunning()) {
    int res;
    std::exception err;
    try {
      res = Minimal_ThrowException(successRate);
    } catch (std::runtime_error e) {
      err = std::move(e);
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_SuccessRate_Minimal_ThrowInt)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
BENCHMARK(BM_SuccessRate_Minimal_ThrowException)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
