#pragma once

#ifndef __has_attribute
# define __has_attribute(x) 0
#endif

#if !defined(GNUC_PREREQ)
#if defined(__GNUC__) && defined(__GNUC_MINOR__) && defined(__GNUC_PATCHLEVEL__)
#define GNUC_PREREQ(maj, min, patch)                                  \
  ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) + __GNUC_PATCHLEVEL__ >=          \
   ((maj) << 20) + ((min) << 10) + (patch))
#elif defined(__GNUC__) && defined(__GNUC_MINOR__)
#define GNUC_PREREQ(maj, min, patch)                                  \
  ((__GNUC__ << 20) + (__GNUC_MINOR__ << 10) >= ((maj) << 20) + ((min) << 10))
#else
#define GNUC_PREREQ(maj, min, patch) 0
#endif
#endif

#if __has_attribute(noinline) || GNUC_PREREQ(3, 4, 0)
#define ATTRIBUTE_NOINLINE __attribute__((noinline))
#elif defined(_MSC_VER)
#define ATTRIBUTE_NOINLINE __declspec(noinline)
#else
#define ATTRIBUTE_NOINLINE
#endif

#if __has_attribute(always_inline) || GNUC_PREREQ(4, 0, 0)
#define ATTRIBUTE_ALWAYS_INLINE __attribute__((always_inline))
#elif defined(_MSC_VER)
#define ATTRIBUTE_ALWAYS_INLINE __forceinline
#else
#define ATTRIBUTE_ALWAYS_INLINE
#endif

#undef GNUC_PREREQ

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
