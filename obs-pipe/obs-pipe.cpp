#include <obs-module.h>

OBS_DECLARE_MODULE()

extern void plugin_register();

bool obs_module_load(void) {
  plugin_register();
  return true;
}
