#pragma once

#include <stdlib.h>

/**
  @brief Returns a random floating point number in [-1, 1].
 */
static inline float randf() {
  int r = rand();
  return (float)r / (float)RAND_MAX;
}