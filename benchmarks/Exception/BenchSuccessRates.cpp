#include "FastRand.h"

#include <benchmark/benchmark.h>

#include <stdexcept>

// -----------------------------------------------------------------------------

__attribute__((noinline))
static int NoExcept(int input) noexcept {
  if (input % 3 == 0) // 3, 6, 9, 12, ...
    return input + 2;

  if (input % 6 != 0) // 1, 2, 4, 5, 7, 8, 10, 11, ...
    return input - 3;

  return input; // never
}

void BM_NoExcept(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res = NoExcept(fastrand());
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

__attribute__((noinline))
static int SuccessAlways_ThrowInt(int input) {
  if (input % 3 == 0) // 3, 6, 9, 12, ...
    return input + 2;

  if (input % 6 != 0) // 1, 2, 4, 5, 7, 8, 10, 11, ...
    return input - 3;

  throw input; // never
}

void BM_Rate_SuccessAlways_ThrowInt(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    int err;
    try {
      res = SuccessAlways_ThrowInt(fastrand());
    } catch (int e) {
      err = e;
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static int Success2outof3_ThrowInt(int input) {
  if (input % 3 == 0) // 3, 6, 9, 12, ...
    return input + 2;

  if (input % 2 == 0) // 2, 4, 8, 10, ...
    return input - 3;

  throw input; // 1, 5, 7, 11, ...
}

void BM_Rate_Success2outOf3_ThrowInt(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    int err;
    try {
      res = Success2outof3_ThrowInt(fastrand());
    } catch (int e) {
      err = e;
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static int Success1outof3_ThrowInt(int input) {
  if (input % 6 == 0) // 6, ...
    return input + 2;

  if (input % 3 == 0) // 3, 9, ...
    return input - 3;

  throw input; // 1, 2, 4, 5, 7, 8 ...
}

void BM_Rate_Success1outOf3_ThrowInt(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    int err;
    try {
      res = Success1outof3_ThrowInt(fastrand());
    } catch (int e) {
      err = e;
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static int SuccessNever_ThrowInt(int input) {
  if (input % 3 == 3) // never
    return input + 2;

  if (input % 6 > 5) // never
    return input - 3;

  throw input; // always
}

void BM_Rate_SuccessNever_ThrowInt(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    int err;
    try {
      res = SuccessNever_ThrowInt(fastrand());
    } catch (int e) {
      err = e;
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

__attribute__((noinline))
static int SuccessAlways_ThrowException(int input) {
  if (input % 3 == 0) // 3, 6, 9, 12, ...
    return input + 2;

  if (input % 6 != 0) // 1, 2, 4, 5, 7, 8, 10, 11, ...
    return input - 3;

  throw std::runtime_error("Mocked Error"); // never
}

void BM_Rate_SuccessAlways_ThrowException(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    std::exception err;
    try {
      res = SuccessAlways_ThrowException(fastrand());
    } catch (std::runtime_error e) {
      err = std::move(e);
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static int Success2outof3_ThrowException(int input) {
  if (input % 3 == 0) // 3, 6, 9, 12, ...
    return input + 2;

  if (input % 2 == 0) // 2, 4, 8, 10, ...
    return input - 3;

  throw std::runtime_error("Mocked Error"); // 1, 5, 7, 11, ...
}

void BM_Rate_Success2outOf3_ThrowException(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    std::exception err;
    try {
      res = Success2outof3_ThrowException(fastrand());
    } catch (std::runtime_error e) {
      err = std::move(e);
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static int Success1outof3_ThrowException(int input) {
  if (input % 6 == 0) // 6, ...
    return input + 2;

  if (input % 3 == 0) // 3, 9, ...
    return input - 3;

  throw std::runtime_error("Mocked Error"); // 1, 2, 4, 5, 7, 8 ...
}

void BM_Rate_Success1outOf3_ThrowException(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    std::exception err;
    try {
      res = Success1outof3_ThrowException(fastrand());
    } catch (std::runtime_error e) {
      err = std::move(e);
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

__attribute__((noinline))
static int SuccessNever_ThrowException(int input) {
  if (input % 3 == 3) // never
    return input + 2;

  if (input % 6 > 5) // never
    return input - 3;

  throw std::runtime_error("Mocked Error"); // always
}

void BM_Rate_SuccessNever_ThrowException(benchmark::State &state) {
  while (state.KeepRunning()) {
    int res;
    std::exception err;
    try {
      res = SuccessNever_ThrowException(fastrand());
    } catch (std::runtime_error e) {
      err = std::move(e);
    }
    benchmark::DoNotOptimize(err);
    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK(BM_NoExcept);

BENCHMARK(BM_Rate_SuccessAlways_ThrowInt);
BENCHMARK(BM_Rate_Success2outOf3_ThrowInt);
BENCHMARK(BM_Rate_Success1outOf3_ThrowInt);
BENCHMARK(BM_Rate_SuccessNever_ThrowInt);

BENCHMARK(BM_Rate_SuccessAlways_ThrowException);
BENCHMARK(BM_Rate_Success2outOf3_ThrowException);
BENCHMARK(BM_Rate_Success1outOf3_ThrowException);
BENCHMARK(BM_Rate_SuccessNever_ThrowException);
