#pragma once

#include <stddef.h>

#include "alloc.h"

typedef struct {
  char* begin;
  char* end;
} pico_arena_t;

pico_arena_t pico_arena_from_buf(void* buf, size_t capacity);
pico_arena_t pico_arena_suballoc(pico_arena_t* a, size_t capacity);
pico_alloc_t pico_arena_allocator(pico_arena_t* a);

void* pico_arena_malloc(pico_arena_t* a, size_t size, size_t align);
void pico_arena_free(pico_arena_t* a, void* ptr, size_t size, size_t align);
void* pico_arena_realloc(pico_arena_t* a, void* ptr, size_t old_size,
                         size_t new_size, size_t align);