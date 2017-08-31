#include "ForceAllErrors.h"

#include "Errors.h"

#include <iostream>

#ifndef NDEBUG
#define DEBUG(X) X
#else
#define DEBUG(X)
#endif

namespace llvm {

ForceAllErrors ForceAllErrors::GlobalInstance;

bool ForceAllErrors::TurnInstanceIntoError() {
  ForceAllErrors &FAE = getInstance();

  if (FAE.Guard) {
    if (++FAE.InstanceCount == FAE.InstanceToBreak) {
      //DEBUG(sys::PrintStackTrace(std::cerr, 6));
      return true;
    }
  }

  return false;
}

std::unique_ptr<ErrorInfoBase> ForceAllErrors::mockError() {
  return std::make_unique<llvm::StringError>(
      "Mocked Error", llvm::inconvertibleErrorCode());
}

std::error_code ForceAllErrors::mockErrorCode() {
  return std::error_code(-1, std::generic_category());
}

ForceAllErrorsInScope::ForceAllErrorsInScope(int ForceErrorNumber)
    : Mode(getModeFromClArg(ForceErrorNumber)) {
  ForceAllErrors &FAE = ForceAllErrors::getInstance();

  switch (Mode) {
  case Count:
    FAE.BeginCounting(this);
    break;
  case Break:
    FAE.BeginBreakInstance(ForceErrorNumber, this);
    DEBUG(std::cerr << "Aim to break instance #" << ForceErrorNumber << "\n");
    break;
  case Disabled:
    break;
  }
}

ForceAllErrorsInScope::~ForceAllErrorsInScope() {
  ForceAllErrors &FAE = ForceAllErrors::getInstance();

  if (Mode == Count)
    DEBUG(std::cerr << FAE.getCount());

  FAE.End();
}
}
