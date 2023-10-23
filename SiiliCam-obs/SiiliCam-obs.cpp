#include <obs-module.h>
#include <obs-frontend-api.h>
#include <Processing.NDI.Lib.h>
#include "restserver/RestServer.hpp"
#include <thread>
#include "Logger.hpp"
#include "siilicam-source.h"
#include "shared-data.h"

using namespace std;
OBS_DECLARE_MODULE();
OBS_MODULE_USE_DEFAULT_LOCALE("siilicam", "en-US");



// Enumeration function for sources
static void EnumSourcesProc(obs_source_t* parent, obs_source_t* child, void* param) {
	// Get source name and type
	const char* sourceName = obs_source_get_name(child);
	const char* sourceType = obs_source_get_id(child);

	// Print source information
	blog(LOG_INFO, "Source name: %s, Type: %s", sourceName, sourceType);

	// If you want to print more data about each source, you can do so here
}


void PrintSourcesOfActiveScene() {
	// Get the active scene
	obs_source_t* currentScene = obs_frontend_get_current_scene();
	if (!currentScene) {
		blog(LOG_INFO, "No active scene found.");
		return;
	}

	// Get the name of the active scene
	const char* sceneName = obs_source_get_name(currentScene);
	blog(LOG_INFO, "Active scene: %s", sceneName);
	// Enumerate sources in the scene
	obs_source_enum_active_sources(currentScene, EnumSourcesProc, nullptr);

	// Release the reference to the active scene
	obs_source_release(currentScene);
}

void GetCurrentScenes()
{
	obs_frontend_source_list scenes = {};

	obs_frontend_get_scenes(&scenes);

	for (size_t i = 0; i < scenes.sources.num; i++) {
		obs_source_t* scene = scenes.sources.array[i];
		const char* name = obs_source_get_name(scene);
		blog(LOG_INFO, "Scene name: %s", name);
	}

	obs_frontend_source_list_free(&scenes);
}

static void on_event(enum obs_frontend_event event, void* private_data)
{
	if (event == OBS_FRONTEND_EVENT_SCENE_LIST_CHANGED) {
		GetCurrentScenes();
	}
	else if (event == OBS_FRONTEND_EVENT_SCENE_CHANGED) {
		PrintSourcesOfActiveScene();
	}
}
void logging(const std::string& logMessage) {
	blog(LOG_INFO, logMessage.c_str());
}
bool obs_module_load(void)
{
	Logger::Logger::getInstance().setLogFunction(logging);
	Logger::init_logging("C:/Users/Simo/AppData/Roaming/log2.log");
	Logger::log_info("siilicam plugin loaded");
	blog(LOG_INFO, "siilicam plugin loaded");
	GetCurrentScenes(); // Print scenes when the plugin is loaded

	// Connect to the scene list changed event
	obs_frontend_add_event_callback(on_event, nullptr);
	Logger::log_info("siilicam plugin loaded");
	obs_register_source(&siilicam_source_info);
	return true;
}


void obs_module_unload(void)
{
	NDIlib_destroy();
	blog(LOG_INFO, "siilicam plugin unloaded");
}
