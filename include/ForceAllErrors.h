#pragma once

#include <cassert>
#include <memory>
#include <system_error>

namespace llvm {

class ErrorInfoBase;
class ForceAllErrorsInScope;

class ForceAllErrors {
public:
  ForceAllErrors() = default;

  void BeginCounting(ForceAllErrorsInScope *Caller) {
    assert(Guard == nullptr);
    Guard = Caller;
    InstanceCount = 0;
    InstanceToBreak = 0;
  }

  void BeginBreakInstance(int ForceErrorNumber, ForceAllErrorsInScope *Caller) {
    assert(Guard == nullptr);
    Guard = Caller;
    InstanceCount = 0;
    InstanceToBreak = ForceErrorNumber;
  }

  void End() {
    assert(Guard != nullptr);
    Guard = nullptr;
  }

  int getCount() const { return InstanceCount; }

  static ForceAllErrors &getInstance() { return GlobalInstance; }

  static bool TurnInstanceIntoError();

  static std::unique_ptr<ErrorInfoBase> mockError();
  static std::error_code mockErrorCode();

private:
  int InstanceCount = 0;
  int InstanceToBreak = 0;
  ForceAllErrorsInScope *Guard = nullptr;
  static ForceAllErrors GlobalInstance;
};

class ForceAllErrorsInScope {
  enum ModeT { Disabled = 0, Count, Break };

public:
  ForceAllErrorsInScope(int ForceErrorNumber);
  ~ForceAllErrorsInScope();

  bool isModeCounting() const { return Mode == Count; }

  int getNumMutationPoints() const {
    assert(isModeCounting());
    return ForceAllErrors::getInstance().getCount();
  }

private:
  ModeT Mode;

  ModeT getModeFromClArg(int ForceErrorNumber) {
    if (ForceErrorNumber < 0)
      return Disabled;

    if (ForceErrorNumber > 0)
      return Break;

    return Count;
  }
};

}
