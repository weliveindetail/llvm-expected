#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <benchmark/benchmark.h>

#include <chrono>
#include <system_error>
#include <thread>

using namespace llvm;

// -----------------------------------------------------------------------------
// helpers

std::string getErrorDescription(std::error_code ec) noexcept {
  return (ec == std::errc::invalid_argument) ? "invalid_argument"
                                             : "unknown error";
}

class GlobPattern {
public:
  GlobPattern() noexcept {
    Gt10 = fastrand() % 10 + 100;
  }

  static Expected<GlobPattern> create(std::string input) noexcept {
    if (fastrand() % 10 > Gt10) // never happens
      return make_error<StringError>(
          "invalid glob pattern: " + input,
          std::make_error_code(std::errc::invalid_argument));

    return GlobPattern();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int Gt10;
};

class GlobPatternEC {
public:
  GlobPatternEC() noexcept {
    Gt10 = fastrand() % 10 + 100;
  }

  static std::error_code create(std::string input, GlobPatternEC &result) noexcept {
    if (fastrand() % 10 > Gt10) // never happens
      return std::make_error_code(std::errc::invalid_argument);

    result = GlobPatternEC();
    return std::error_code();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int Gt10;
};

class GlobPatternAdHoc {
public:
  GlobPatternAdHoc() noexcept {
    Gt10 = fastrand() % 10 + 100;
  }

  static bool create(std::string input, GlobPatternAdHoc &result) noexcept {
    if (fastrand() % 10 > Gt10) // never happens
      return false;

    result = GlobPatternAdHoc();
    return true;
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return (fastrand() % 10 > 4);
  }

private:
  static int Gt10;
};

int GlobPattern::Gt10;
int GlobPatternEC::Gt10;
int GlobPatternAdHoc::Gt10;

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE Expected<bool> OverheadExample_Expected_Fwd() noexcept {
  return OverheadExample_Expected_Fwd<N - 1>();
}

template <>
ATTRIBUTE_NOINLINE Expected<bool> OverheadExample_Expected_Fwd<0>() noexcept {
  std::string fileName = "[a*.txt";

  Expected<GlobPattern> pattern = GlobPattern::create(std::move(fileName));
  if (!pattern)
    return pattern.takeError();

  return pattern->match("...");
}

template <int N>
void BM_OverheadExample_Expected_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    Expected<bool> res = OverheadExample_Expected_Fwd<N>();
    if (auto err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[OverheadExample] ");
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE std::error_code OverheadExample_ErrorCode_Fwd(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  return OverheadExample_ErrorCode_Fwd<N - 1>(result, errorFileName);
}

template <>
ATTRIBUTE_NOINLINE std::error_code OverheadExample_ErrorCode_Fwd<0>(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  GlobPatternEC pattern;
  std::string fileName = "[a*.txt";

  if (std::error_code ec = GlobPatternEC::create(fileName, pattern)) {
    errorFileName = std::make_unique<std::string>(fileName);
    return ec;
  }

  result = pattern.match("...");
  return std::error_code();
}

template <int N>
void BM_OverheadExample_ErrorCode_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    bool res;
    std::unique_ptr<std::string> errorFileName = nullptr; // only instantiate in error case

    if (std::error_code ec = OverheadExample_ErrorCode_Fwd<N>(res, errorFileName)) {
      nulls << "[OverheadExample] " << getErrorDescription(ec) << " ";
      nulls << *errorFileName << "\n";
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

template <int N>
ATTRIBUTE_NOINLINE bool OverheadExample_AdHoc_Fwd(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  return OverheadExample_AdHoc_Fwd<N - 1>(result, errorFileName);
}

template <>
ATTRIBUTE_NOINLINE bool OverheadExample_AdHoc_Fwd<0>(
    bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  GlobPatternAdHoc pattern;
  std::string fileName = "[a*.txt";

  if (!GlobPatternAdHoc::create(fileName, pattern)) {
    errorFileName = std::make_unique<std::string>(fileName);
    return false;
  }

  result = pattern.match("...");
  return true;
}

template <int N>
void BM_OverheadExample_AdHoc_Fwd(benchmark::State &state) {
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    bool res;
    std::unique_ptr<std::string> errorFileName = nullptr; // only instantiate in error case

    if (!OverheadExample_AdHoc_Fwd<N>(res, errorFileName)) {
      nulls << "[OverheadExample] " << *errorFileName << "\n";
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

BENCHMARK_TEMPLATE1(BM_OverheadExample_Expected_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_OverheadExample_Expected_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_OverheadExample_Expected_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_OverheadExample_Expected_Fwd, 16);

BENCHMARK_TEMPLATE1(BM_OverheadExample_ErrorCode_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_OverheadExample_ErrorCode_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_OverheadExample_ErrorCode_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_OverheadExample_ErrorCode_Fwd, 16);

BENCHMARK_TEMPLATE1(BM_OverheadExample_AdHoc_Fwd, 0);
BENCHMARK_TEMPLATE1(BM_OverheadExample_AdHoc_Fwd, 2);
BENCHMARK_TEMPLATE1(BM_OverheadExample_AdHoc_Fwd, 8);
BENCHMARK_TEMPLATE1(BM_OverheadExample_AdHoc_Fwd, 16);
