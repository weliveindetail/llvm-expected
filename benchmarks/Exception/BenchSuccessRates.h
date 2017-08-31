#include <benchmark/benchmark.h>

void BM_NoExcept(benchmark::State &state);

void BM_Rate_SuccessAlways_ThrowInt(benchmark::State &state);
void BM_Rate_Success2outOf3_ThrowInt(benchmark::State &state);
void BM_Rate_Success1outOf3_ThrowInt(benchmark::State &state);
void BM_Rate_SuccessNever_ThrowInt(benchmark::State &state);

void BM_Rate_SuccessAlways_ThrowException(benchmark::State &state);
void BM_Rate_Success2outOf3_ThrowException(benchmark::State &state);
void BM_Rate_Success1outOf3_ThrowException(benchmark::State &state);
void BM_Rate_SuccessNever_ThrowException(benchmark::State &state);
