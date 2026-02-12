#include "args.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

#include "arena.h"

static pico_err_t parse_int32(const char* s, int32_t* p) {
  char* end;
  int32_t i = strtol(s, &end, 10);
  if (errno == ERANGE || end == s || *end != '\0') {
    return PICO_ERR;
  }
  *p = i;
  return PICO_OK;
}

static pico_err_t parse_int64(const char* s, int64_t* p) {
  char* end;
  int64_t i = strtoll(s, &end, 10);
  if (errno == ERANGE || end == s || *end != '\0') {
    return PICO_ERR;
  }
  *p = i;
  return PICO_OK;
}

pico_err_t pico_args_parse(const pico_args_t* args, int argc, char** argv,
                           pico_arena_t scratch) {
  // TODO
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