#pragma once

#include "Compiler.h"

#if defined(_MSC_VER)

#include <intrin.h>
inline void UseInt(int volatile) {}

// Helper function with stable runtime
ATTRIBUTE_ALWAYS_INLINE
inline int workload(int val) noexcept {
  UseInt(val);
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  __nop();
  return val;
}

#else

// Helper function with stable runtime
ATTRIBUTE_ALWAYS_INLINE
inline int workload(int val) noexcept {
  asm volatile(
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    "nop \n"
    : : "g"(val) : );
  return val;
}
#endif
