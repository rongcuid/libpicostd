#include "errors.h"
const char *pico_err_str(pico_err_t err) {
  switch (err) {
    case PICO_OK:
      return "ok";
    case PICO_ERR:
      return "generic error";
    case PICO_BUG:
      return "bug";
    case PICO_TODO:
      return "TODO";
    case PICO_BADARGS:
      return "bad arguments";
    case PICO_NOMEM:
      return "out of memory";
    case PICO_DEV_ERR:
      return "generic device error";
    case PICO_DEV_NOMEM:
      return "device out of memory";
    default:
      return "unknown error";
  }
}