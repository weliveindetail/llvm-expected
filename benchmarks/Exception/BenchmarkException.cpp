#include <benchmark/benchmark.h>
#include <common/outcome.hpp>
#include <string>

int main(int argc, char **argv) {

#if ENABLE_BENCHMARK_OUTCOME
  printf("sizeof(outcome::result<int>) = %lu\n",
         sizeof(OUTCOME_V2_NAMESPACE::result<int>));

  printf("sizeof(outcome::result<int, std::string>) = %lu\n",
         sizeof(OUTCOME_V2_NAMESPACE::result<int, std::string>));
#endif

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
