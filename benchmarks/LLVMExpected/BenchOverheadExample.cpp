#include "BenchOverheadExample.h"
#include "FastRand.h"

#include <Errors.h>
#include <Expected.h>

#include <chrono>
#include <system_error>
#include <thread>

using namespace llvm;

// -----------------------------------------------------------------------------
// helpers

bool simulateSomeWork() noexcept {
  int v = 0;
  for (int i = 0; i < 1; i++)
    v += fastrand() % 10;

  return v % 2 == 0;
}

std::string getErrorDescription(std::error_code ec) noexcept {
  return (ec == std::errc::invalid_argument) ? "invalid_argument"
                                             : "unknown error";
}

class GlobPatternEC {
public:
  static std::error_code create(std::string input, GlobPatternEC &result) noexcept {
    if (fastrand() % 100 > SuccessRate)
      return std::make_error_code(std::errc::invalid_argument);

    result = GlobPatternEC();
    return std::error_code();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return simulateSomeWork();
  }

  // helper function for benchmark
  static void setSuccessRate(int percent) noexcept { SuccessRate = percent; }

private:
  static int SuccessRate;
};

class GlobPattern {
public:
  static Expected<GlobPattern> create(std::string input) noexcept {
    if (fastrand() % 100 > SuccessRate)
      return make_error<StringError>(
          "invalid glob pattern: " + input,
          std::make_error_code(std::errc::invalid_argument));

    return GlobPattern();
  }

  bool match(std::string text) const noexcept {
    benchmark::DoNotOptimize(text);
    return simulateSomeWork();
  }

  // helper function for benchmark
  static void setSuccessRate(int percent) noexcept { SuccessRate = percent; }

private:
  static int SuccessRate;
};

int GlobPatternEC::SuccessRate;
int GlobPattern::SuccessRate;

// -----------------------------------------------------------------------------

std::error_code
OverheadExample_ErrorCode(bool &result, std::unique_ptr<std::string> &errorFileName) noexcept {
  GlobPatternEC pattern;
  std::string fileName = "[a*.txt";

  if (std::error_code ec = GlobPatternEC::create(fileName, pattern)) {
    errorFileName = std::make_unique<std::string>(fileName);
    return ec;
  }

  result = pattern.match("...");
  return std::error_code();
}

void BM_SuccessRate_OverheadExample_ErrorCode(benchmark::State &state) {
  GlobPatternEC::setSuccessRate(state.range(0));
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    bool res;
    std::unique_ptr<std::string> errorFileName = nullptr; // only instantiate in error case

    if (std::error_code ec = OverheadExample_ErrorCode(res, errorFileName)) {
      nulls << "[simpleExample Error] " << getErrorDescription(ec) << " ";
      nulls << *errorFileName << "\n";
    }

    benchmark::DoNotOptimize(res);
  }
}

// -----------------------------------------------------------------------------

Expected<bool> OverheadExample_Expected() noexcept {
  std::string fileName = "[a*.txt";

  Expected<GlobPattern> pattern = GlobPattern::create(std::move(fileName));
  if (!pattern)
    return pattern.takeError();

  return pattern->match("...");
}

void BM_SuccessRate_OverheadExample_Expected(benchmark::State &state) {
  GlobPattern::setSuccessRate(state.range(0));
  std::ostringstream nulls;

  while (state.KeepRunning()) {
    Expected<bool> res = OverheadExample_Expected();
    if (auto err = res.takeError()) {
      logAllUnhandledErrors(std::move(err), nulls, "[simpleExample Error] ");
    }

    benchmark::DoNotOptimize(res);
  }
}
