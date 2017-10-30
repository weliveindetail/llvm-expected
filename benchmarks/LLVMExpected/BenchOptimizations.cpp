#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

// -----------------------------------------------------------------------------

struct A {
  int value;
};

ATTRIBUTE_NOINLINE
static A MinStruct_NoInline_Raw(int input) noexcept {
  return {input};
}

ATTRIBUTE_NOINLINE
static llvm::Expected<A> MinStruct_NoInline_ExpectedSuccess(int input) noexcept {
  return A{input};
}

ATTRIBUTE_NOINLINE
static llvm::Expected<A> MinStruct_NoInline_ExpectedFail(int input) noexcept {
  return llvm::make_error<llvm::StringError>("mock", llvm::inconvertibleErrorCode());
}

void REF_MinStruct_NoInline_Raw() noexcept {
  auto res = MinStruct_NoInline_Raw(fastrand_NoInline());
  benchmark::DoNotOptimize(res);
}

void BM_MinStruct_NoInline_Raw(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_MinStruct_NoInline_Raw();
  }
}

void REF_MinStruct_NoInline_ExpectedSuccess() noexcept {
  auto res = MinStruct_NoInline_ExpectedSuccess(fastrand_NoInline());

#ifndef NDEBUG
  if (!res)
    llvm::consumeError(res.takeError());
#endif

  benchmark::DoNotOptimize(res);
}

void BM_MinStruct_NoInline_ExpectedSuccess(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_MinStruct_NoInline_ExpectedSuccess();
  }
}

void REF_MinStruct_NoInline_ExpectedFail() noexcept {
  auto res = MinStruct_NoInline_ExpectedFail(fastrand_NoInline());

#ifndef NDEBUG
  if (!res)
    llvm::consumeError(res.takeError());
#endif

  benchmark::DoNotOptimize(res);
}

void BM_MinStruct_NoInline_ExpectedFail(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_MinStruct_NoInline_ExpectedFail();
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
static void NoRVO_OutParam(std::vector<int> &v) {
  v.reserve(8);
  v[0] = fastrand();
  v[1] = fastrand();
  v[2] = fastrand();
  v[3] = fastrand();
  v[4] = fastrand();
  v[5] = fastrand();
  v[6] = fastrand();
  v[7] = fastrand();
}

void REF_NoRVO_OutParam() noexcept {
  std::vector<int> res;
  NoRVO_OutParam(res);
  benchmark::DoNotOptimize(res);
}

void BM_NoRVO_OutParam(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_NoRVO_OutParam();
  }
}

ATTRIBUTE_NOINLINE
static std::vector<int> NoRVO_Move() noexcept {
  std::vector<int> res {
    fastrand(), fastrand(), fastrand(), fastrand(),
    fastrand(), fastrand(), fastrand(), fastrand()
  };
  return std::move(res); // warning here is intentional
}

void REF_NoRVO_Move() noexcept {
  std::vector<int> res = NoRVO_Move();
  benchmark::DoNotOptimize(res);
}

void BM_NoRVO_Move(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_NoRVO_Move();
  }
}

ATTRIBUTE_NOINLINE
static std::vector<int> NoRVO_Copy() noexcept {
  std::vector<int> res {
    fastrand(), fastrand(), fastrand(), fastrand(),
    fastrand(), fastrand(), fastrand(), fastrand()
  };
  return std::vector<int>{res};
}

void REF_NoRVO_Copy() noexcept {
  std::vector<int> res = NoRVO_Copy();
  benchmark::DoNotOptimize(res);
}

void BM_NoRVO_Copy(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_NoRVO_Copy();
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
static std::vector<int> RVO_Raw() {
  return {
    fastrand(), fastrand(), fastrand(), fastrand(),
    fastrand(), fastrand(), fastrand(), fastrand()
  };
}

void REF_RVO_Raw() noexcept {
  std::vector<int> res = RVO_Raw();
  benchmark::DoNotOptimize(res);
}

void BM_RVO_Raw(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_RVO_Raw();
  }
}

ATTRIBUTE_NOINLINE
static llvm::Expected<std::vector<int>> RVO_Expected() {
  return std::vector<int>{
    fastrand(), fastrand(), fastrand(), fastrand(),
    fastrand(), fastrand(), fastrand(), fastrand()
  };
}

void REF_RVO_Expected() noexcept {
  auto res = RVO_Expected();

#ifndef NDEBUG
  if (!res)
    llvm::consumeError(res.takeError());
#endif

  benchmark::DoNotOptimize(res);
}

void BM_RVO_Expected(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_RVO_Expected();
  }
}

// -----------------------------------------------------------------------------

ATTRIBUTE_NOINLINE
static std::vector<int> NRVO_Raw() noexcept {
  std::vector<int> res {
    fastrand(), fastrand(), fastrand(), fastrand(),
    fastrand(), fastrand(), fastrand(), fastrand()
  };
  return res;
}

void REF_NRVO_Raw() noexcept {
  std::vector<int> res = NRVO_Raw();
  benchmark::DoNotOptimize(res);
}

void BM_NRVO_Raw(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_NRVO_Raw();
  }
}

ATTRIBUTE_NOINLINE
static llvm::Expected<std::vector<int>> NRVO_Expected() {
  std::vector<int> res {
    fastrand(), fastrand(), fastrand(), fastrand(),
    fastrand(), fastrand(), fastrand(), fastrand()
  };
  return std::move(res);
}

void REF_NRVO_Expected() noexcept {
  auto res = NRVO_Expected();

#ifndef NDEBUG
  if (!res)
    llvm::consumeError(res.takeError());
#endif

  benchmark::DoNotOptimize(res);
}

void BM_NRVO_Expected(benchmark::State &state) noexcept {
  while (state.KeepRunning()) {
    REF_NRVO_Expected();
  }
}

// -----------------------------------------------------------------------------

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
