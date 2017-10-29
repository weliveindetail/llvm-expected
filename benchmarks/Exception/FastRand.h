#pragma once
#include <time.h>

static int g_seed;
static struct SeedInit { SeedInit() { g_seed = time(NULL); } } g_seed_init;

__attribute__((always_inline))
inline int fastrand() noexcept {
  g_seed = (214013*g_seed+2531011); 
  return (g_seed>>16)&0x7FFF; 
}

__attribute__((noinline))
inline int fastrand_NoInline() noexcept {
  g_seed = (214013*g_seed+2531011); 
  return (g_seed>>16)&0x7FFF; 
}
