#include <benchmark/benchmark.h>
#include <outcome.hpp>
#include <string>

int main(int argc, char **argv) {
  printf("sizeof(outcome::result<int>) = %lu\n",
         sizeof(OUTCOME_V2_NAMESPACE::result<int>));

  printf("sizeof(outcome::result<int, std::string>) = %lu\n",
         sizeof(OUTCOME_V2_NAMESPACE::result<int, std::string>));

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
