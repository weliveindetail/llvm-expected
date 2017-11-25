#include "common/Compiler.h"
#include "common/FastRand.h"
#include "common/WorkloadSim.h"

#include <Errors.h>
#include <Expected.h>
#include <benchmark/benchmark.h>

using namespace llvm;

namespace FwdVoidError {

template <int N>
ATTRIBUTE_NOINLINE Error IMPL_FwdVoidError(int gt10) noexcept {
  gt10 = workload(gt10);

  if (auto err = IMPL_FwdVoidError<N - 1>(gt10))
    return err; // never happens

  workload(0);
  return Error::success();
}

template <>
ATTRIBUTE_NOINLINE Error IMPL_FwdVoidError<1>(int gt10) noexcept {
  if (workload(gt10) < 10)
    return llvm::make_error<llvm::StringError>(
        "Mocked Error", llvm::inconvertibleErrorCode()); // never happens

  workload(0);
  return Error::success();
}

template <int N>
void BM_FwdVoidError(benchmark::State &state) {
  std::ostringstream nulls;
  int gt10 = fastrand() % 10 + 100;

  while (state.KeepRunning()) {
    if (Error err = IMPL_FwdVoidError<N>(gt10)) {
      logAllUnhandledErrors(std::move(err), nulls, "[never happens]");
    }
  }
}

BENCHMARK_TEMPLATE1(BM_FwdVoidError, 1);
BENCHMARK_TEMPLATE1(BM_FwdVoidError, 2);
BENCHMARK_TEMPLATE1(BM_FwdVoidError, 4);
BENCHMARK_TEMPLATE1(BM_FwdVoidError, 8);

}
