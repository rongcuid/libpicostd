#pragma once

#include <stdbool.h>

#define expect(x, ...)              \
  do {                              \
    if ((x)) {                      \
      fprintf(stderr, __VA_ARGS__); \
      exit(EXIT_FAILURE);           \
    }                               \
  } while (false)
