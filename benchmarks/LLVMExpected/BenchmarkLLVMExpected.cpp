#include <benchmark/benchmark.h>
#include <Expected.h>
#include <system_error>

int main(int argc, char **argv) {
  printf("sizeof(int) = %lu\n", sizeof(int));
  printf("sizeof(std::error_code) = %lu\n", sizeof(std::error_code));
  printf("sizeof(llvm::Error) = %lu\n", sizeof(llvm::Error));
  printf("sizeof(llvm::Expected<int>) = %lu\n", sizeof(llvm::Expected<int>));

  benchmark::Initialize(&argc, argv);
  benchmark::RunSpecifiedBenchmarks();
}
