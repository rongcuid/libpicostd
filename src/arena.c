#include "arena.h"

#include <assert.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

arena_t arena_from_buf(void *buf, size_t capacity) {
  arena_t a = {0};
  a.begin = buf;
  a.end = a.begin + capacity;
  return a;
}

arena_t arena_suballoc(arena_t *a, size_t capacity) {
  arena_t b = {0};
  return arena_from_buf(arena_malloc(a, capacity, 1), capacity);
}

/**
    Credits: Chris Wellons:
    * https://nullprogram.com/blog/2023/09/27/
    * https://nullprogram.com/blog/2023/12/17/
    Modified.
 */
void *arena_malloc(arena_t *a, size_t size, size_t align) {
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
void arena_free(arena_t *a, void *ptr, size_t size, size_t align) {
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
void *arena_realloc(arena_t *a, void *ptr, size_t old_size, size_t new_size,
                    size_t align) {
  // This just allocates a new region without reclaiming memory
  assert(new >= old);
  void *r = arena_malloc(a, new_size, align);
  return memcpy(r, ptr, old_size);
}