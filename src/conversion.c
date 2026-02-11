#include "conversion.h"

int64_t ts_to_ns(const struct timespec *ts) {
  int64_t ns = ts->tv_nsec;
  ns += (int64_t)ts->tv_sec * 1000000000LL;
  return ns;
}