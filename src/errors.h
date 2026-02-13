#pragma once

#include <stdlib.h>

/** @brief Main error type for pico std. All fallable functions return this.*/
typedef enum {
  PICO_OK = 0,
  PICO_ERR = 1,
  PICO_NOMEM = 2,
  PICO_BADARGS = 3,
} pico_err_t;

#define pt_expect(x, fail_msg) \
  if ((errno = (x))) {         \
    perror(fail_msg);          \
    exit(EXIT_FAILURE);        \
  }
