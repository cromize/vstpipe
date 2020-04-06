#include <obs.h>
#include <obs-source.h>

static void *plugin_create(obs_data_t *settings, obs_source_t *source) {

}

static void plugin_destroy() {

}

static const char *plugin_getname(void *unused) {
  UNUSED_PARAMETER(unused);
  return "vstpipe OBS client";
}

struct obs_source_info socket_source = {
  .id = "socket_source",
  .type = OBS_SOURCE_TYPE_INPUT,
  .output_flags = OBS_SOURCE_AUDIO,
  .get_name = plugin_getname,
  .create = plugin_create,
  .destroy = plugin_destroy,
};