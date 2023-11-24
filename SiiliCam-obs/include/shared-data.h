#pragma once

#include <vector>
#include <mutex>
#include <graphics.h> // Required for rendering functions
#include "NDIReceiver.hpp"
#include <obs-module.h>

struct custom_data {
    std::string obs_source_name;
    std::string selected_ndi_source; // The NDI source selected by the user
    obs_source_t* source;
    std::shared_ptr<NDIReceiver> ndiReceiver;
    uint32_t width = 1200;  // Default values
    uint32_t height = 500;  // Default values
    int32_t aspect_ratio_width = 50;
    int32_t aspect_ratio_height = 50;
    float zoom = 0.0f;
};
extern std::vector<custom_data*> sharedCustomDataVector;
extern std::mutex customDataMutex; // Mutex to protect sharedCustomDataVector
