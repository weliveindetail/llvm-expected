#include <benchmark/benchmark.h>

void BM_Rate_SuccessAlways_ErrorCode(benchmark::State &state);
void BM_Rate_SuccessAlways_Expected(benchmark::State &state);

void BM_Rate_Success2outOf3_ErrorCode(benchmark::State &state);
void BM_Rate_Success2outOf3_Expected(benchmark::State &state);

void BM_Rate_Success1outOf3_ErrorCode(benchmark::State &state);
void BM_Rate_Success1outOf3_Expected(benchmark::State &state);

void BM_Rate_SuccessNever_ErrorCode(benchmark::State &state);
void BM_Rate_SuccessNever_Expected(benchmark::State &state);
