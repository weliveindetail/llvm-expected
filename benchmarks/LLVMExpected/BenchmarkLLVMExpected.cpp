#include "BenchPerfCategories.h"
#include "BenchOptimizations.h"
#include "BenchOverheadExample.h"
#include "BenchSuccessRates.h"

#include <benchmark/benchmark.h>

BENCHMARK(BM_Category_FastRand);
BENCHMARK(BM_Category_FastRand_BranchInstruction);
BENCHMARK(BM_Category_FastRand_FunctionCall);
BENCHMARK(BM_Category_FastRand_FunctionPtrCall);
BENCHMARK(BM_Category_HeapAllocDealloc);
BENCHMARK(BM_Category_HeapAllocDealloc_UniquePtrMove);
BENCHMARK(BM_Category_HeapAllocDealloc_SharedPtrMove);
BENCHMARK(BM_Category_HeapAllocDealloc_SharedPtrCopy);
BENCHMARK(BM_Category_DiskReadAccess);

BENCHMARK(BM_MinStruct_NoInline_Raw);
BENCHMARK(BM_MinStruct_NoInline_ExpectedSuccess);
BENCHMARK(BM_MinStruct_NoInline_ExpectedFail);

BENCHMARK(BM_NoRVO_OutParam);
BENCHMARK(BM_NoRVO_Move);
BENCHMARK(BM_NoRVO_Copy);
BENCHMARK(BM_RVO_Raw);
BENCHMARK(BM_RVO_Expected);
BENCHMARK(BM_NRVO_Raw);
BENCHMARK(BM_NRVO_Expected);

BENCHMARK(BM_Rate_SuccessAlways_ErrorCode);
BENCHMARK(BM_Rate_Success2outOf3_ErrorCode);
BENCHMARK(BM_Rate_Success1outOf3_ErrorCode);
BENCHMARK(BM_Rate_SuccessNever_ErrorCode);

BENCHMARK(BM_Rate_SuccessAlways_Expected);
BENCHMARK(BM_Rate_Success2outOf3_Expected);
BENCHMARK(BM_Rate_Success1outOf3_Expected);
BENCHMARK(BM_Rate_SuccessNever_Expected);

BENCHMARK(BM_SuccessRate_OverheadExample_ErrorCode)->Arg(100)->Arg(66)->Arg(33)->Arg(0);
BENCHMARK(BM_SuccessRate_OverheadExample_Expected)->Arg(100)->Arg(66)->Arg(33)->Arg(0);

int main(int argc, char **argv) {
  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
