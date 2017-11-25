#include "common/Compiler.h"
#include "common/FastRand.h"

#include <Errors.h>
#include <Expected.h>
#include <benchmark/benchmark.h>
#include <system_error>

using namespace llvm;

namespace SuccessRateExpected {

ATTRIBUTE_NOINLINE
Expected<int> IMPL_SuccessRateExpected(int successRate) noexcept {
  if (fastrand() % 100 > successRate)
    return errorCodeToError(std::make_error_code(std::errc::invalid_argument));

  return successRate - fastrand() % 10;
}

void BM_SuccessRateExpected(benchmark::State &state) {
  int successRate = state.range(0);
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    auto res = IMPL_SuccessRateExpected(successRate);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[OverheadExample] ");
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK(BM_SuccessRateExpected)->Arg(100)->Arg(95)->Arg(50)->Arg(0);

}
