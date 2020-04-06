#include <obs-module.h>

OBS_DECLARE_MODULE()

extern struct obs_source_info socket_source;

bool obs_module_load(void) {
  obs_register_source(&socket_source);
  return true;
}
