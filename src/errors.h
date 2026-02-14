#pragma once

/** @brief Main error type for pico std. All fallable functions return this.*/
typedef enum {
  /// Success is always 0
  PICO_OK = 0,
  /// Generic error
  PICO_ERR,
  /// Internal error
  PICO_BUG,
  /// Not implemented error
  PICO_TODO,
  /// User error, invalid argument
  PICO_BADARGS,
  /// Memory allocation fail
  PICO_NOMEM,
  /// Device (anything not the host CPU) generic error
  PICO_DEV_ERR,
  /// Device (anything not the host CPU) memory allocation error
  PICO_DEV_NOMEM,
} pico_err_t;

const char *pico_err_str(pico_err_t err);