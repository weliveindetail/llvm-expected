#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <Errors.h>
#include <Expected.h>
#include <benchmark/benchmark.h>

using namespace llvm;

namespace FwdIntExpected {

template <int N>
ATTRIBUTE_NOINLINE Expected<int> IMPL_FwdIntExpected(int gt10) noexcept {
  gt10 = workload(gt10);

  auto res = IMPL_FwdIntExpected<N - 1>(gt10);
  if (!res)
    return res.takeError(); // never happens

  return workload(*res);
}

template <>
ATTRIBUTE_NOINLINE Expected<int> IMPL_FwdIntExpected<1>(int gt10) noexcept {
  if (workload(gt10) < 10)
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode()); // never happens

  return workload(gt10);
}

template <int N>
void BM_FwdIntExpected(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    auto res = IMPL_FwdIntExpected<N>(gt10);

    if (Error err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }

    benchmark::DoNotOptimize(res);
  }
}

BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 1);
BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 2);
BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 4);
BENCHMARK_TEMPLATE1(BM_FwdIntExpected, 8);

}
