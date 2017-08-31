#include "BenchSuccessRates.h"

#include <benchmark/benchmark.h>

BENCHMARK(BM_NoExcept);

BENCHMARK(BM_Rate_SuccessAlways_ThrowInt);
BENCHMARK(BM_Rate_Success2outOf3_ThrowInt);
BENCHMARK(BM_Rate_Success1outOf3_ThrowInt);
BENCHMARK(BM_Rate_SuccessNever_ThrowInt);

BENCHMARK(BM_Rate_SuccessAlways_ThrowException);
BENCHMARK(BM_Rate_Success2outOf3_ThrowException);
BENCHMARK(BM_Rate_Success1outOf3_ThrowException);
BENCHMARK(BM_Rate_SuccessNever_ThrowException);

int main(int argc, char **argv) {
  ::benchmark::Initialize(&argc, argv);
  ::benchmark::RunSpecifiedBenchmarks();
}
