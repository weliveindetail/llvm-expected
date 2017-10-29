#include "FastRand.h"

#include <benchmark/benchmark.h>

#include <stdexcept>

// -----------------------------------------------------------------------------
// helpers

bool simulateSomeWork() noexcept {
  int v = 0;
  for (int i = 0; i < 1; i++)
    v += fastrand() % 10;

  return v % 2 == 0;
}

class GlobPattern {
public:
  static GlobPattern create(std::string input) {
    if (fastrand() % 100 > SuccessRate)
      throw std::runtime_error("invalid glob pattern: " + input);

    return GlobPattern();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return simulateSomeWork();
  }

  // helper function for benchmark
  static void setSuccessRate(int percent) noexcept { SuccessRate = percent; }

private:
  static int SuccessRate;
};

int GlobPattern::SuccessRate;

// -----------------------------------------------------------------------------

bool OverheadExample_ThrowException() {
  std::string fileName = "[a*.txt";

  GlobPattern pattern = GlobPattern::create(std::move(fileName));
  return pattern.match("...");
}

void BM_SuccessRate_OverheadExample_ThrowException(benchmark::State &state) {
  GlobPattern::setSuccessRate(state.range(0));

  while (state.KeepRunning()) {
    bool res;
    std::exception err;
    try {
      res = OverheadExample_ThrowException();
    } catch (std::runtime_error e) {
      err = std::move(e);
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_SuccessRate_OverheadExample_ThrowException)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
