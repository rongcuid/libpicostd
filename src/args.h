#pragma once

#include <stddef.h>
#include <stdint.h>
#include <stdio.h>

#include "arena.h"
#include "errors.h"

typedef enum {
  PICO_ARGS_SWITCH,
  PICO_ARGS_STRING,
  PICO_ARGS_INT32,
  PICO_ARGS_INT64,
} pico_args_kind_t;

typedef struct {
  /// A single-character option flag
  char flag;
  pico_args_kind_t kind;
  /// Writes true if flag is encountered. If nullptr, no-op.
  bool* on;
  /// Pointer to write the parsed value. If nullptr, no-op.
  union {
    void* value_raw;
    int32_t* value_i32;
    int64_t* value_i64;
    const char** value_str;
  };
  /// Optional name of the argument.
  const char* name;
  /// Optional help message.
  const char* help;
} pico_args_switch_t;

typedef struct {
  size_t n_switches;
  pico_args_switch_t* switches;
  /// Optionally write the index after the last parsed options.
  int* last_optind;
  /// Optional description.
  const char* description;
} pico_args_t;

/** @brief Parses argument list. Returns error number.*/
pico_err_t pico_args_parse(const pico_args_t* args, int argc, char** argv,
                           pico_arena_t scratch);

/** @brief Prints usage to file.*/
void pico_args_print_usage(FILE* file, const pico_args_t* args, char* prog);