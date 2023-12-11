#include <obs-module.h>
#include <obs-frontend-api.h>
#include <Processing.NDI.Lib.h>
#include "RestHandlers.h"
#include "RestServer.hpp"
#include <thread>
#include "Logger.hpp"
#include "siilicam-source.h"
#include "shared-data.h"

using namespace std;
OBS_DECLARE_MODULE();
OBS_MODULE_USE_DEFAULT_LOCALE("siilicam", "en-US");

std::shared_ptr<RestServer> server;
std::thread serverThread;


void logging(const std::string& logMessage) {
	blog(LOG_INFO, logMessage.c_str());
}
bool obs_module_load(void)
{
	Logger::Logger::getInstance().setLogFunction(logging);
	Logger::init_logging("siili_log.log");
	Logger::log_info("siilicam plugin loaded");
	
	server = std::make_shared<RestServer>(6042);
	server->addGetHandler("/getSources", getSources);
	server->addPostHandler("/setNDISource", setSource);
	server->addPostHandler("/setFirstMatchingNDISource", setFirstMatchingNdiSource);
	server->addPostHandler("/siiliSourceVisibility", setCameraVisibility);

	serverThread = std::thread([]() {
		server->start();
		});
	// Connect to the scene list changed event
	Logger::log_info("siilicam plugin loaded");
	Logger::log_info("size of source:", sizeof(siilicam_source_info));
	obs_register_source(&siilicam_source_info);
	return true;
}


void obs_module_unload(void)
{
	server->stop();
	serverThread.join();

	blog(LOG_INFO, "siilicam plugin unloaded");

}
