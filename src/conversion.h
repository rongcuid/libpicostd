#pragma once

#include <stdint.h>
#include <time.h>

/** @brief Convert POSIX timespec to nanoseconds. */
int64_t ts_to_ns(const struct timespec *ts);
