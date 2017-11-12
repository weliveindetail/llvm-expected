#include "FastRand.h"

#include <benchmark/benchmark.h>

#include <sstream>
#include <stdexcept>

// -----------------------------------------------------------------------------
// helpers

class GlobPattern {
public:
  static GlobPattern create(std::string input) {
    if (fastrand() % 100 > SuccessRate)
      throw std::runtime_error("invalid glob pattern: " + input);

    return GlobPattern();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

  // helper function for benchmark
  static void setSuccessRate(int percent) noexcept { SuccessRate = percent; }

private:
  static int SuccessRate;
};

int GlobPattern::SuccessRate;

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE bool OverheadExample_ThrowException() {
  std::string fileName = "[a*.txt";

  GlobPattern pattern = GlobPattern::create(std::move(fileName));
  return pattern.match("...");
}

void BM_SuccessRate_OverheadExample_ThrowException(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    bool res;
    try {
      res = OverheadExample_ThrowException();
    } catch (std::runtime_error e) {
      nulls << "[OverheadExample] " << e.what() << "\n";
    }
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_SuccessRate_OverheadExample_ThrowException)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
