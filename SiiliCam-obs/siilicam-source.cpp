#include "siilicam-source.h"
#include <Processing.NDI.Lib.h>
#include <obs-module.h>
#include <graphics.h> // Required for rendering functions
#include <vector>
#include <string>
#include "Logger.hpp"
#include "NDIReceiver.hpp"
#include "shared-data.h"

std::vector<custom_data*> sharedCustomDataVector;
std::mutex customDataMutex;

static obs_properties_t* custom_get_properties(void* data) {
    Logger::log_info("custom properties called");
    blog(LOG_INFO, "custom properties called");
    obs_properties_t* props = obs_properties_create();
    blog(LOG_INFO, "custom properties created");
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);

    obs_property_t* p = obs_properties_add_list(props, "ndi_source_list", "NDI Sources", OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);
    blog(LOG_INFO, "added ndi source list");
    obs_property_list_add_string(p, "TEST", "TEST");
    blog(LOG_INFO, "added test");
    for (const auto& source_name : custom->ndiReceiver->getCurrentSources()) {
        blog(LOG_INFO, (std::string("adding source: ") + source_name).c_str());
        obs_property_list_add_string(p, source_name.c_str(), source_name.c_str());
    }
    return props;
}
gs_texture_t* create_solid_color_texture(uint32_t width, uint32_t height, uint32_t color) {
    gs_texture_t* texture = gs_texture_create(width, height, GS_RGBA, 1, nullptr, GS_DYNAMIC);
    if (!texture) {
        return nullptr;
    }

    uint32_t* data = new uint32_t[width * height];
    for (size_t i = 0; i < width * height; i++) {
        data[i] = color;
    }

    gs_texture_set_image(texture, reinterpret_cast<uint8_t*>(data), width * 4, false);
    delete[] data;
 
    return texture;
}
static void custom_video_render(void* data, gs_effect_t* effect) {
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);
    obs_source_t* source = reinterpret_cast<obs_source_t*>(data);
    if (custom->selected_ndi_source == "TEST") {
        // Update text position
        custom->text_pos.x += custom->text_vel.x;
        custom->text_pos.y += custom->text_vel.y;
        // Boundary check
        if (custom->text_pos.x <= 0 || custom->text_pos.x >=custom->width - 50) {
            custom->text_vel.x = -custom->text_vel.x;
        }
        if (custom->text_pos.y <= 0 || custom->text_pos.y >=custom->height - 50) {
            custom->text_vel.y = -custom->text_vel.y;
        }

        // Create a solid color texture (e.g., red)
        gs_texture_t* solid_texture = create_solid_color_texture(50, 50, 0xFF0000FF); // RGBA
        if (!solid_texture) {
            return; // Failed to create the texture
        }

        // Use the default effect if none is provided
        effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);

        // Push the current matrix onto the stack
        gs_matrix_push();

        // Translate to the desired position
        gs_matrix_translate3f(custom->text_pos.x, custom->text_pos.y, 0.0f);

        // Draw the solid texture at the current position
        gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), solid_texture);
        gs_draw_sprite(solid_texture, 0, 50, 50);

        // Pop the matrix to reset the transformation
        gs_matrix_pop();

        // Clean up the solid texture
        gs_texture_destroy(solid_texture);
    }
    else {

        Image ndiFrame = custom->ndiReceiver->getFrame();

        if (ndiFrame.data.empty()) {
            return;
        }

        const uint8_t* dataPtr = ndiFrame.data.data();
        custom->width = ndiFrame.width;
        custom->height = ndiFrame.height;
        gs_texture_t* ndi_texture = gs_texture_create(ndiFrame.width, ndiFrame.height, GS_RGBA, 1,
            &dataPtr, 0);
        if (!ndi_texture) {
            // Failed to create the texture
            return;
        }

        // Use the default effect if none is provided
        effect = obs_get_base_effect(OBS_EFFECT_DEFAULT);


        // Draw the NDI texture
        gs_effect_set_texture(gs_effect_get_param_by_name(effect, "image"), ndi_texture);
        gs_draw_sprite(ndi_texture, 0, ndiFrame.width, ndiFrame.height);

        // Pop the matrix to reset the transformation (if you pushed one earlier)
        gs_matrix_pop();

        // Clean up the NDI texture
        gs_texture_destroy(ndi_texture);

    }
}



static void custom_update(void* data, obs_data_t* settings) {
    Logger::log_info("update called");
    blog(LOG_INFO, "update called");
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);

    blog(LOG_INFO, "getting the ndi source list");
    custom->selected_ndi_source = obs_data_get_string(settings, "ndi_source_list");
    if (custom->selected_ndi_source == "TEST") {
        custom->text_pos.x =custom->width / 2.0;
        custom->text_pos.y =custom->height / 2.0;
        blog(LOG_INFO, "stopping frame generation");
        Logger::log_info("stopping frame generation");
        custom->ndiReceiver->stopFrameGeneration();
        Logger::log_info("stopped");
        blog(LOG_INFO, "stopped frame generation");
    }
    else {
        blog(LOG_INFO, "starting frame generation");
        Logger::log_info("starting frame generation");
        custom->ndiReceiver->setOutput(custom->selected_ndi_source);
        custom->ndiReceiver->startFrameGeneration();
        Logger::log_info("frame generation started");
        blog(LOG_INFO, "started frame generation");
    }
    Logger::log_info("selected ndi source in update:", custom->selected_ndi_source);
}

static void* custom_create(obs_data_t* settings, obs_source_t* source) {
    Logger::log_info("starting the Siilicam OBS Source");
    blog(LOG_INFO, "starting the Siilicam OBS Source");
    struct custom_data* data = new custom_data;
    data->ndiReceiver = std::make_shared<NDIReceiver>("SiiliCam", true);
    blog(LOG_INFO, "created ndi receiver");
    data->ndiReceiver->start();
    blog(LOG_INFO, "ndi receiver started");
    // Initialize text position in the center of the source
    data->text_pos.x =data->width / 2.0;
    data->text_pos.y =data->height / 2.0;

    // Initialize text velocity (you can adjust these values)
    data->text_vel.x = -5.0 * 2;
    data->text_vel.y = 8.6 * 2;
    std::lock_guard<std::mutex> lock(customDataMutex);
    sharedCustomDataVector.push_back(data);
    blog(LOG_INFO, "going to call update");
    custom_update(data, settings);
    blog(LOG_INFO, "done");
    return data;
}

static void custom_destroy(void* data) {
    Logger::log_info("destroy called");
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);
    Logger::log_info("going to delete custom");
    delete custom;
    Logger::log_info("custom deleted");
    std::lock_guard<std::mutex> lock(customDataMutex);
    sharedCustomDataVector.erase(std::remove(sharedCustomDataVector.begin(), sharedCustomDataVector.end(), data), sharedCustomDataVector.end());
}

// TODO: Implement other necessary callbacks like video_render, etc.
static uint32_t custom_get_width(void* data) {
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);
    return custom->width;
}

static uint32_t custom_get_height(void* data) {
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);
    return custom->height;
}
struct obs_source_info siilicam_source_info = {
    .id = "siilicamobs_source",
    .type = OBS_SOURCE_TYPE_INPUT,
    .output_flags = OBS_SOURCE_VIDEO,
    .get_name = [](void*) -> const char* { return "Siili Cam Source"; },
    .create = custom_create,
    .destroy = custom_destroy,
    .get_width = custom_get_width,       // Add this line
    .get_height = custom_get_height,
    .get_properties = custom_get_properties,
    .update = custom_update,
    .video_render = custom_video_render,
    // ... [Other necessary callbacks and fields]
};