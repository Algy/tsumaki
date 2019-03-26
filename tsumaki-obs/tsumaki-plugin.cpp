#include <obs-module.h>

OBS_DECLARE_MODULE()
/* Implements common ini-based locale (optional) */
OBS_MODULE_USE_DEFAULT_LOCALE("my-plugin", "en-US")


extern struct obs_source_info tsumaki_filter;
bool obs_module_load(void) {
	obs_register_source(&tsumaki_filter);
	return true;
}

