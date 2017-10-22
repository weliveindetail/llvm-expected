#pragma once

#include <memory>
#include <string>

#include <gtest/gtest.h>

#include <Expected.h>
#include <Errors.h>

class ExtraPayload {
  bool Checked = false;
  friend void consumeExtraPayload(std::unique_ptr<ExtraPayload>);

public:
  ExtraPayload() = default;
  ExtraPayload(ExtraPayload&&) = default;

  ~ExtraPayload() {
    EXPECT_TRUE(Checked);
  }
};

inline std::unique_ptr<ExtraPayload> makeExtraPayload() {
  return std::make_unique<ExtraPayload>();
}

inline void consumeExtraPayload(std::unique_ptr<ExtraPayload> ep) {
  ep->Checked = true;
}

/// This class represents some custom Error.
class ExtraPayloadError : public llvm::ErrorInfo<ExtraPayloadError> {
public:
  static char ID;

  ExtraPayloadError(std::unique_ptr<ExtraPayload> EP)
    : EP(std::move(EP)) {}

  void log(std::ostream &OS) const override {
    OS << "ExtraPayloadError\n";
  }

  std::error_code convertToErrorCode() const override {
    return llvm::inconvertibleErrorCode();
  }

  std::unique_ptr<ExtraPayload> takeExtraPayload() {
    return std::move(EP);
  }

private:
  std::unique_ptr<ExtraPayload> EP;
};
