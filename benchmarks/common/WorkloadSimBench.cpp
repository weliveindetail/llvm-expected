#include "common/WorkloadSim.h"

#include <benchmark/benchmark.h>
#include <numeric>
#include <vector>

// Measure runtime and noise ratio of workload simulation

void BM_100xWorkloadBindConst(benchmark::State &state) {
  while (state.KeepRunning()) {
    int i = 100;
    do {
      workload(0);
    } while (--i != 0);
  }
}

void BM_100xWorkloadBindVariable(benchmark::State &state) {
  while (state.KeepRunning()) {
    int i = 100;
    do {
      workload(i);
    } while (--i != 0);
  }
}

static double calc_mean_single(const std::vector<double>& v) {
  return (std::accumulate(std::begin(v), std::end(v), 0.0) / 100.0) / v.size();
}

BENCHMARK(BM_100xWorkloadBindConst)
  ->Repetitions(32)
  ->ReportAggregatesOnly(true)
  ->ComputeStatistics("mean_single", calc_mean_single);

BENCHMARK(BM_100xWorkloadBindVariable)
  ->Repetitions(32)
  ->ReportAggregatesOnly(true)
  ->ComputeStatistics("mean_single", calc_mean_single);
