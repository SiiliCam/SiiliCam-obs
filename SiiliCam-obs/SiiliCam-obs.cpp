#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#include "Logger.hpp"

#include "RestHandlers.h"
#include "RestServer.hpp"
#include "shared-data.h"
#include "siilicam-source.h"
#include <filesystem>
#include <obs-frontend-api.h>
#include <obs-module.h>
#include <thread>

using namespace std;
OBS_DECLARE_MODULE();
OBS_MODULE_USE_DEFAULT_LOCALE("siilicam", "en-US");

std::shared_ptr<RestServer> server;
std::thread serverThread;

static bool set_env_ndi(const std::string &dir) {
#if defined(_WIN32)
  return _putenv_s("NDI_CONFIG_DIR", dir.c_str());
#else
  return setenv("NDI_CONFIG_DIR", dir.c_str(), 1) == 0;
#endif
}

static bool try_set_from_local_cfg() {
  char *cfg_c = obs_module_config_path("ndi-config.v1.json");
  if (!cfg_c)
    return false;
  std::filesystem::path cfg{cfg_c};

  bfree(cfg_c);

  if (!std::filesystem::exists(cfg)) {
    Logger::log_info("Path does not exist:", cfg.string());
    _putenv("NDI_CONFIG_DIR=");
    return false;
  }
  Logger::log_info("Path does exists:", cfg.string());
  return set_env_ndi(cfg.parent_path().string());
}

void logging(const std::string &logMessage) {
  blog(LOG_INFO, logMessage.c_str());
}
bool obs_module_load(void) {

  Logger::Logger::getInstance().setLogFunction(logging);
  Logger::init_logging("siili_log.log");

  Logger::log_info("siilicam plugin loaded");
  if (try_set_from_local_cfg()) {
    Logger::log_info("Env loaded");
  }

  server = std::make_shared<RestServer>(6042);
  server->addGetHandler("/getSources", getSources);
  server->addPostHandler("/setNDISource", setSource);
  server->addPostHandler("/setFirstMatchingNDISource",
                         setFirstMatchingNdiSource);
  server->addPostHandler("/siiliSourceVisibility", setCameraVisibility);
  server->addPostHandler("/setText", setTextSource);
  serverThread = std::thread([]() { server->start(); });
  // Connect to the scene list changed event
  Logger::log_info("siilicam plugin loaded");
  Logger::log_info("size of source:", sizeof(siilicam_source_info));
  obs_register_source(&siilicam_source_info);
  return true;
}

void obs_module_unload(void) {
  server->stop();
  serverThread.join();

  blog(LOG_INFO, "siilicam plugin unloaded");
}
