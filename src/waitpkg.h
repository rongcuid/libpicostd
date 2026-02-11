#pragma once

#include <stdbool.h>
#include <stdint.h>

#include "errors.h"

////// Diagnostics

bool has_waitpkg(void);
bool cpu_umonitor_mitg_no(void);
long long umwait_max_time(void);
bool umwait_enable_c02(void);
void report_waitpkg(void);

////// Utilities

typedef enum {
  WAITPKG_C01 = 1,
  WAITPKG_C02 = 2,
} waitpkg_cstate_t;

pico_err_t waitpkg_cstate_to_control(waitpkg_cstate_t c_state,
                                     uint32_t *control);