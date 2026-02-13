#include "args.h"

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "arena.h"

static pico_err_t parse_int32(const char* s, int32_t* p) {
  errno = 0;
  char* end;
  long i = strtol(s, &end, 10);
  if (errno == ERANGE || end == s || *end != '\0' || i < INT32_MIN ||
      i > INT32_MAX) {
    return PICO_ERR;
  }
  *p = (int32_t)i;
  return PICO_OK;
}

static pico_err_t parse_int64(const char* s, int64_t* p) {
  errno = 0;
  char* end;
  long long i = strtoll(s, &end, 10);
  if (errno == ERANGE || end == s || *end != '\0' || i < INT64_MIN ||
      i > INT64_MAX) {
    return PICO_ERR;
  }
  *p = (int64_t)i;
  return PICO_OK;
}

pico_err_t pico_args_parse(const pico_args_t* args, int argc, char** argv,
                           pico_arena_t scratch) {
  (void)scratch;
  if (args == nullptr || argv == nullptr || argc < 0) {
    return PICO_ERR;
  }

  int optind = 1;
  while (optind < argc) {
    char* token = argv[optind];
    if (token == nullptr) {
      return PICO_ERR;
    }

    // End-of-options marker.
    if (strcmp(token, "--") == 0) {
      ++optind;
      break;
    }

    // First non-option stops parsing.
    if (token[0] != '-' || token[1] == '\0') {
      break;
    }

    // Parse short-option cluster, e.g. -abc or -n42.
    size_t off = 1;
    while (token[off] != '\0') {
      char flag = token[off];
      pico_args_switch_t* sw = nullptr;
      for (size_t i = 0; i < args->n_switches; ++i) {
        if (args->switches[i].flag == flag) {
          sw = &args->switches[i];
          break;
        }
      }
      if (sw == nullptr) {
        return PICO_ERR;
      }

      if (sw->on != nullptr) {
        *sw->on = true;
      }

      if (sw->kind == PICO_ARGS_SWITCH) {
        ++off;
        continue;
      }

      const char* value = nullptr;
      if (token[off + 1] != '\0') {
        // Inline value form: -sVALUE
        value = &token[off + 1];
      } else {
        // Separate value form: -s VALUE
        if (optind + 1 >= argc || argv[optind + 1] == nullptr) {
          return PICO_ERR;
        }
        if (strcmp(argv[optind + 1], "--") == 0) {
          return PICO_ERR;
        }
        value = argv[++optind];
      }

      if (sw->kind == PICO_ARGS_INT32) {
        int32_t tmp = 0;
        int32_t* out = sw->value_i32 != nullptr ? sw->value_i32 : &tmp;
        if (parse_int32(value, out) != PICO_OK) {
          return PICO_ERR;
        }
      } else if (sw->kind == PICO_ARGS_INT64) {
        int64_t tmp = 0;
        int64_t* out = sw->value_i64 != nullptr ? sw->value_i64 : &tmp;
        if (parse_int64(value, out) != PICO_OK) {
          return PICO_ERR;
        }
      } else if (sw->kind == PICO_ARGS_STRING) {
        if (sw->value_str != nullptr) {
          *sw->value_str = value;
        }
      } else {
        return PICO_ERR;
      }

      // Value consumes remainder of this token (or next argv), so this token
      // is fully handled.
      break;
    }

    ++optind;
  }

  if (args->last_optind != nullptr) {
    *args->last_optind = optind;
  }
  return PICO_OK;
}

void pico_args_print_usage(FILE* file, const pico_args_t* args, char* prog) {
  fprintf(file, "Usage: %s [OPTION]...\n", prog);
  if (args->description != nullptr) {
    fprintf(file, "%s\n", args->description);
  }
  if (args->n_switches > 0) {
    fprintf(file, "OPTIONS:\n");
    for (size_t i = 0; i < args->n_switches; ++i) {
      const pico_args_switch_t* s = &args->switches[i];
      fprintf(file, "\t-%c", s->flag);
      if (s->name != nullptr) {
        fprintf(file, "%s", s->name);
      }
      if (s->help != nullptr) {
        fprintf(file, "\t%s", s->help);
      }
      fprintf(file, "\n");
    }
  }
}
