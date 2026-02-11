#pragma once

#include <stddef.h>

typedef struct {
  char *begin;
  char *end;
} arena_t;

arena_t arena_from_buf(void *buf, size_t capacity);
arena_t arena_suballoc(arena_t *a, size_t capacity);
void *arena_malloc(arena_t *a, size_t size, size_t align);
void arena_free(arena_t *a, void *ptr, size_t size, size_t align);
void *arena_realloc(arena_t *a, void *ptr, size_t old_size, size_t new_size,
                    size_t align);