#pragma once

#include <benchmark/benchmark.h>

void REF_MinStruct_NoInline_Raw() noexcept;
void REF_MinStruct_NoInline_Expected() noexcept;

void BM_MinStruct_NoInline_Raw(benchmark::State &state) noexcept;
void BM_MinStruct_NoInline_ExpectedSuccess(benchmark::State &state) noexcept;
void BM_MinStruct_NoInline_ExpectedFail(benchmark::State &state) noexcept;

void REF_MoveCtor_Delete() noexcept;
void BM_MoveCtor_Delete(benchmark::State &state) noexcept;

void REF_NoRVO_OutParam() noexcept;
void REF_NoRVO_Move() noexcept;
void REF_NoRVO_Copy() noexcept;
void BM_NoRVO_OutParam(benchmark::State &state) noexcept;
void BM_NoRVO_Move(benchmark::State &state) noexcept;
void BM_NoRVO_Copy(benchmark::State &state) noexcept;

void REF_RVO_Raw() noexcept;
void REF_RVO_Expected() noexcept;
void BM_RVO_Raw(benchmark::State &state) noexcept;
void BM_RVO_Expected(benchmark::State &state) noexcept;

void REF_NRVO_Raw() noexcept;
void REF_NRVO_Expected() noexcept;
void BM_NRVO_Raw(benchmark::State &state) noexcept;
void BM_NRVO_Expected(benchmark::State &state) noexcept;
