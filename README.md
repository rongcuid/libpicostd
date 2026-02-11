# libpicostd

The `libpicostd` is meant to be a tiny, cross-platform C extended standard library for the explicit purpose of helping writing microbenchmarks.

## Goals

- All memory allocation are explicit.
- Include utilities commonly used by microbenchmarks, such as `hwloc`.
- Be lightweight.
- Be reentrant.