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

/*
static int make_input() noexcept {
  using namespace std::chrono;
  auto t = system_clock::now().time_since_epoch();
  return duration_cast<milliseconds>(t).count();
}

static int no_except(int value) noexcept {
  if (value % 3 == 0) // 3, 6, 9, 12, ...
    return value + 2;

  if (value % 6 != 0) // 1, 2, 4, 5, 7, 8, 10, 11, ...
    return value - 3;

  return 0; // never
}

// -----------------------------------------------------------------------------

namespace throw_int {

static int success_always(int value) {
  if (value % 3 == 0) // 3, 6, 9, 12, ...
    return value + 2;

  if (value % 6 != 0) // 1, 2, 4, 5, 7, 8, 10, 11, ...
    return value - 3;

  throw value; // never
}

static int success_2outof3(int value) {
  if (value % 3 == 0) // 3, 6, 9, 12, ...
    return value + 2;

  if (value % 2 == 0) // 2, 4, 8, 10, ...
    return value - 3;

  throw value; // 1, 5, 7, 11, ...
}

static int success_1outof3(int value) {
  if (value % 6 == 0) // 6, ...
    return value + 2;

  if (value % 3 == 0) // 3, 9, ...
    return value - 3;

  throw value; // 1, 2, 4, 5, 7, 8, ...
}

static int success_never(int value) {
  if (value % 3 == 3) // never
    return value + 2;

  if (value % 6 > 5) // never
    return value - 3;

  throw value; // always
}
}

// -----------------------------------------------------------------------------

namespace throw_except {

static int success_always(int value) {
  if (value % 3 == 0) // 3, 6, 9, 12, ...
    return value + 2;

  if (value % 6 != 0) // 1, 2, 4, 5, 7, 8, 10, 11, ...
    return value - 3;

  throw std::system_error(std::error_code(-1, std::system_category()),
                          "message"); // never
}

static int success_2outof3(int value) {
  if (value % 3 == 0) // 3, 6, 9, 12, ...
    return value + 2;

  if (value % 2 == 0) // 2, 4, 8, 10, ...
    return value - 3;

  throw std::system_error(std::error_code(-1, std::system_category()),
                          "message"); // 1, 5, 7, 11, ...
}

static int success_1outof3(int value) {
  if (value % 6 == 0) // 6, ...
    return value + 2;

  if (value % 3 == 0) // 3, 9, ...
    return value - 3;

  throw std::system_error(std::error_code(-1, std::system_category()),
                          "message"); // 1, 2, 4, 5, 7, 8, ...
}

static int success_never(int value) {
  if (value % 3 == 3) // never
    return value + 2;

  if (value % 6 > 5) // never
    return value - 3;

  throw std::system_error(std::error_code(-1, std::system_category()),
                          "message"); // always
}
}
*/
