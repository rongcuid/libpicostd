#include "waitpkg.h"

#include <cpuid.h>
#include <stdio.h>
#include <stdlib.h>
#include <x86intrin.h>

bool has_waitpkg() {
  unsigned eax, ebx, ecx, edx;
  if (!__get_cpuid_max(0, 0)) return false;
  __cpuid_count(7, 0, eax, ebx, ecx, edx);
  return (ecx & (1u << 5)) != 0;  // WAITPKG
}

bool cpu_umonitor_mitg_no() {
  unsigned eax, ebx, ecx, edx;
  __cpuid_count(7, 2, eax, ebx, ecx, edx);
  return (edx & (1u << 7)) != 0;  // MONITOR_MITG_NO
}

long long umwait_max_time() {
  FILE *f = fopen("/sys/devices/system/cpu/umwait_control/max_time", "r");
  if (f == NULL) {
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  size_t len = ftell(f);
  char *buf = malloc(len);
  fseek(f, 0, SEEK_SET);
  fread(buf, 1, len, f);
  long long max_time = atoll(buf);
  fclose(f);
  return max_time;
}

bool umwait_enable_c02() {
  FILE *f = fopen("/sys/devices/system/cpu/umwait_control/enable_c02", "r");
  if (f == NULL) {
    exit(1);
  }
  fseek(f, 0, SEEK_END);
  size_t len = ftell(f);
  char *buf = malloc(len);
  fseek(f, 0, SEEK_SET);
  fread(buf, 1, len, f);
  long enable_c02 = atol(buf);
  fclose(f);
  return enable_c02;
}

void report_waitpkg() {
  bool has_wp = has_waitpkg();
  bool mitigation = cpu_umonitor_mitg_no();
  fprintf(stderr, "CPU has waitpkg: %d; has MON_UMON_MITG_SUPPORT: %d\n",
          (int)has_wp, (int)mitigation);
  long long max_time = umwait_max_time();
  bool c02 = umwait_enable_c02();
  fprintf(stderr, "umwait max wait time: %llu; C0.2 enable: %d\n", max_time,
          c02);
}

pico_err_t waitpkg_cstate_to_control(waitpkg_cstate_t c_state,
                                     uint32_t *control) {
  bool ok = false;
  switch (c_state) {
    case 1:
      *control = 1;
      break;
    case 2:
      *control = 0;
      break;
    default:
      return PICO_ERR;
  }
  return PICO_OK;
}