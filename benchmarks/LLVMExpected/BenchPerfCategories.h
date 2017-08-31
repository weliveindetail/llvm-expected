#pragma once

#include <benchmark/benchmark.h>

void BM_Category_FastRand(benchmark::State &state) noexcept;
void BM_Category_FastRand_BranchInstruction(benchmark::State &state) noexcept;
void BM_Category_FastRand_FunctionCall(benchmark::State &state) noexcept;
void BM_Category_FastRand_FunctionPtrCall(benchmark::State &state) noexcept;
void BM_Category_HeapAllocDealloc(benchmark::State &state) noexcept;
void BM_Category_HeapAllocDealloc_UniquePtrMove(benchmark::State &state) noexcept;
void BM_Category_HeapAllocDealloc_SharedPtrMove(benchmark::State &state) noexcept;
void BM_Category_HeapAllocDealloc_SharedPtrCopy(benchmark::State &state) noexcept;
void BM_Category_DiskReadAccess(benchmark::State &state) noexcept;
