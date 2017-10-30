#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

#include <system_error>

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
static std::error_code Minimal_ErrorCode(int successRate, int &res) noexcept {
  if (fastrand() % 100 > successRate)
    return std::error_code(9, std::system_category());

  res = successRate;
  return std::error_code();
}

void BM_SuccessRate_Minimal_ErrorCode(benchmark::State &state) {
  int successRate = state.range(0);

  while (state.KeepRunning()) {
    int res;
    auto ec = Minimal_ErrorCode(successRate, res);

    benchmark::DoNotOptimize(ec);
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
static llvm::Expected<int> Minimal_Expected(int successRate) noexcept {
  if (fastrand() % 100 > successRate)
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode());

  return successRate;
}

void BM_SuccessRate_Minimal_Expected(benchmark::State &state) {
  int successRate = state.range(0);

  while (state.KeepRunning()) {
    auto res = Minimal_Expected(successRate);

#ifdef NDEBUG
    benchmark::DoNotOptimize(res);
#else
    llvm::consumeError(res.takeError());
#endif
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_SuccessRate_Minimal_ErrorCode)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
BENCHMARK(BM_SuccessRate_Minimal_Expected)->Arg(100)->Arg(95)->Arg(50)->Arg(0);
