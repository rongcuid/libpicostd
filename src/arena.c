#include "arena.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "alloc.h"

static void* arena_alloc_vt_malloc(void* ctx, size_t size, size_t align) {
  return pico_arena_malloc((pico_arena_t*)ctx, size, align);
}

static void arena_alloc_vt_free(void* ctx, void* ptr, size_t size,
                                size_t align) {
  pico_arena_free((pico_arena_t*)ctx, ptr, size, align);
}

static void* arena_alloc_vt_realloc(void* ctx, void* ptr, size_t old_size,
                                    size_t new_size, size_t align) {
  return pico_arena_realloc((pico_arena_t*)ctx, ptr, old_size, new_size, align);
}

static const pico_alloc_vt arena_alloc_vt = {
    .malloc = arena_alloc_vt_malloc,
    .free = arena_alloc_vt_free,
    .realloc = arena_alloc_vt_realloc,
};

pico_alloc_t pico_arena_allocator(pico_arena_t* a) {
  return (pico_alloc_t){
      .vt = &arena_alloc_vt,
      .context = a,
  };
}

pico_arena_t pico_arena_from_buf(void* buf, size_t capacity) {
  pico_arena_t a = {0};
  a.begin = buf;
  a.end = a.begin + capacity;
  return a;
}

pico_arena_t pico_arena_suballoc(pico_arena_t* a, size_t capacity) {
  pico_arena_t b = {0};
  return pico_arena_from_buf(pico_arena_malloc(a, capacity, 1), capacity);
}

/**
    Credits: Chris Wellons:
    * https://nullprogram.com/blog/2023/09/27/
    * https://nullprogram.com/blog/2023/12/17/
    Modified.
 */
void* pico_arena_malloc(pico_arena_t* a, size_t size, size_t align) {
  // This arena allocator allocates from the end.
  ptrdiff_t available = a->end - a->begin;
  ptrdiff_t padding = -(ptrdiff_t)size & (align - 1);
  if (size > available - padding) {
    return NULL;
  }
  return a->end -= size + padding;
}

/**
    Credits: Chris Wellons:
    * https://nullprogram.com/blog/2023/09/27/
    * https://nullprogram.com/blog/2023/12/17/
    Modified.
 */
void pico_arena_free(pico_arena_t* a, void* ptr, size_t size, size_t align) {
  // Only the last object reclaims memory when freed.
  if (ptr == a->end) {
    ptrdiff_t padding = -size & (align - 1);
    a->end += size + padding;
  }
}

/**
    Credits: Chris Wellons:
    * https://nullprogram.com/blog/2023/09/27/
    * https://nullprogram.com/blog/2023/12/17/
    Modified.
 */
void* pico_arena_realloc(pico_arena_t* a, void* ptr, size_t old_size,
                         size_t new_size, size_t align) {
  // This just allocates a new region without reclaiming memory
  assert(new_size >= old_size);
  void* r = pico_arena_malloc(a, new_size, align);
  return memcpy(r, ptr, old_size);
}