#include "alloc.h"

#include <stdlib.h>

static void* c_malloc(void* context, size_t size, size_t align) {
  (void)context;
  (void)align;
  return malloc(size);
}

static void c_free(void* context, void* ptr, size_t size, size_t align) {
  (void)context;
  (void)size;
  (void)align;
  free(ptr);
}

static void* c_realloc(void* context, void* ptr, size_t old_size,
                       size_t new_size, size_t align) {
  (void)context;
  (void)old_size;
  (void)align;
  return realloc(ptr, new_size);
}

static const pico_alloc_vt pico_c_allocator_vt = {
    .malloc = &c_malloc,
    .free = &c_free,
    .realloc = &c_realloc,
};

pico_alloc_t pico_cstdlib_allocator() {
  return (pico_alloc_t){.vt = &pico_c_allocator_vt, .context = nullptr};
}
