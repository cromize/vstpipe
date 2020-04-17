#include <obs.h>
#include <obs-source.h>
#include <pipe.h>

static void *plugin_create(obs_data_t *settings, obs_source_t *source) {
  return NULL;
}

static void plugin_destroy(void *data) {

}

static const char *plugin_getname(void *unused) {
  UNUSED_PARAMETER(unused);
  return "vstpipe OBS client";
}

void plugin_register() {
  obs_source_info info = {};
  info.id = "socket_source";
  info.type = OBS_SOURCE_TYPE_INPUT;
  info.output_flags = OBS_SOURCE_AUDIO;
  info.get_name = plugin_getname;
  info.create = plugin_create;
  info.destroy = plugin_destroy;
  obs_register_source(&info);
}
