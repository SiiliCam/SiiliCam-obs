#pragma once

#include <vector>
#include <mutex>
#include <graphics.h> // Required for rendering functions
#include "NDIReceiver.hpp"
#include <obs-module.h>

struct custom_data {
    std::string selected_ndi_source; // The NDI source selected by the user
    vec2 text_pos; // Current position of the text
    vec2 text_vel; // Velocity of the text (how much it moves per frame)
    std::shared_ptr<NDIReceiver> ndiReceiver;
    uint32_t width = 1200;  // Default values
    uint32_t height = 500;  // Default values
};
extern std::vector<custom_data*> sharedCustomDataVector;
extern std::mutex customDataMutex; // Mutex to protect sharedCustomDataVector
