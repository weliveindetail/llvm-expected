#pragma once

#include "Compiler.h"
#include <time.h>

static int g_seed;
static struct SeedInit { SeedInit() { g_seed = time(NULL); } } g_seed_init;

ATTRIBUTE_ALWAYS_INLINE
inline int fastrand() noexcept {
  g_seed = (214013 * g_seed + 2531011);
  return (g_seed >> 16) & 0x7FFF;
}

ATTRIBUTE_NOINLINE
inline int fastrand_NoInline() noexcept {
  g_seed = (214013 * g_seed + 2531011);
  return (g_seed >> 16) & 0x7FFF;
}
