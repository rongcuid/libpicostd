#include "args.h"

#include <errno.h>
#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "arena.h"

static char* make_getopt_optstring(const pico_args_t* args, pico_arena_t* up,
                                   pico_arena_t scratch) {
  // Set to 1 due to the NUL character.
  size_t optstring_len = 1;
  // First, iterate through all arguments to collect flags.
  for (int i = 0; i < args->n_switches; ++i) {
    const pico_args_switch_t* sw = &args->switches[i];
    if (sw->kind == PICO_ARGS_SWITCH) {
      // Append just the flag character.
      optstring_len += 1;
    } else {
      // Append flag and a following character.
      optstring_len += 2;
    }
  }
  // Populate the string.
  char* str = arena_malloc(up, optstring_len, 1);
  if (str == nullptr) return nullptr;
  // Next, write the string.
  for (int i = 0, j = 0; i < args->n_switches; ++i) {
    const pico_args_switch_t* sw = &args->switches[i];
    // Write the flag first.
    str[j++] = sw->flag;
    if (sw->kind != PICO_ARGS_SWITCH) {
      str[j++] = ':';
    }
  }
  str[optstring_len - 1] = '\0';
  return str;
}

static pico_err_t parse_with_getopt(const pico_args_t* args,
                                    const char* optstring, int argc,
                                    char** argv) {
  int opt;
  while ((opt = getopt(argc, argv, optstring)) != -1) {
    bool found = false;
    for (int i = 0; i < args->n_switches; ++i) {
      const pico_args_switch_t* sw = &args->switches[i];
      if (sw->flag != opt) continue;
      found = true;
      if (sw->on != nullptr) *sw->on = true;
      if (sw->value_raw != nullptr) {
        char* end;
        switch (sw->kind) {
          case PICO_ARGS_STRING:
            *sw->value_str = optarg;
            break;
          case PICO_ARGS_INT32: {
            int32_t i = strtol(optarg, &end, 10);
            if (errno == ERANGE || end == optarg || *end != '\0') {
              fprintf(stderr, "Invalid int32 number: %s\n", optarg);
              return PICO_ERR;
            }
            *sw->value_i32 = i;
            break;
          }
          case PICO_ARGS_INT64: {
            int64_t i = strtoll(optarg, &end, 10);
            if (errno == ERANGE || end == optarg || *end != '\0') {
              fprintf(stderr, "Invalid int64 number: %s\n", optarg);
              return PICO_ERR;
            }
            *sw->value_i64 = i;
            break;
          }
          default:
        }
      }
      break;
    }
    if (!found) {
      fprintf(stderr, "Invalid argument: -%c\n", opt);
      return PICO_ERR;
    }
  }
  if (args->option_index != nullptr) *args->option_index = optind;
  return PICO_OK;
}

pico_err_t pico_args_parse(const pico_args_t* args, int argc, char** argv,
                           pico_arena_t scratch) {
  pico_arena_t sub_scratch = arena_suballoc(&scratch, 1024);
  const char* optstring = make_getopt_optstring(args, &scratch, sub_scratch);
  if (optstring == nullptr) return PICO_NOMEM;
  return parse_with_getopt(args, optstring, argc, argv);
}

void pico_args_print_usage(FILE* file, const pico_args_t* args, char* prog) {
  fprintf(file, "Usage: %s [OPTION]...\n", prog);
  // TODO
}