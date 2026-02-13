#pragma once

#include <stddef.h>

typedef struct pico_alloc_t pico_alloc_t;

typedef struct {
  void* (*malloc)(void* context, size_t size, size_t align);
  void (*free)(void* context, void* ptr, size_t size, size_t align);
  void* (*realloc)(void* context, void* ptr, size_t old_size, size_t new_size,
                   size_t align);
} pico_alloc_vt;

struct pico_alloc_t {
  const pico_alloc_vt* vt;
  void* context;
};

/** The C stdlib memory allocator.*/
pico_alloc_t pico_cstdlib_allocator(void);

static inline void* pico_malloc(pico_alloc_t self, size_t size, size_t align) {
  return self.vt->malloc(self.context, size, align);
}
static inline void pico_free(pico_alloc_t self, void* ptr, size_t size,
                             size_t align) {
  self.vt->free(self.context, ptr, size, align);
}
static inline void* pico_realloc(pico_alloc_t self, void* ptr, size_t old_size,
                                 size_t new_size, size_t align) {
  return self.vt->realloc(self.context, ptr, old_size, new_size, align);
}
