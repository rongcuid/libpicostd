#pragma once

#include <stddef.h>

typedef struct {
  void* context;
  void* (*malloc)(void* context, size_t size, size_t align);
  void (*free)(void* context, void* ptr, size_t size, size_t align);
  void* (*realloc)(void* context, size_t old_size, size_t new_size,
                   size_t align);
} pico_alloc_t;