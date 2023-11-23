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

static bool switch_camera_button_clicked(obs_properties_t* props, obs_property_t* property, void* data)
{
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);
    SwitchCamera cam{ true };
    custom->ndiReceiver->sendMetadata(cam);
    // Your logic for switching the camera here.
    // For example: custom->ndiReceiver->switchCamera();

    return true; // Return true to indicate that properties should be refreshed (optional).
}

static bool aspect_ratio_button_clicked(obs_properties_t* props, obs_property_t* property, void* data, int width, int height) {
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);
    AspectRatio as{ width, height };
    custom->ndiReceiver->sendMetadata(as);
    obs_source_t* source = obs_get_source_by_name(custom->obs_source_name.c_str());
    if (!source) {
        // Handle the case where the source cannot be found
        return false;
    }

    obs_data_t* settings = obs_source_get_settings(source);
    custom->aspect_ratio_height = height;
    custom->aspect_ratio_height = width;
    if (width > 0 && height > 0) {

        obs_data_set_int(settings, "aspect_ratio_width", width);
        obs_data_set_int(settings, "aspect_ratio_height", height);
    }
    else {
        obs_data_set_int(settings, "aspect_ratio_width", 1);
        obs_data_set_int(settings, "aspect_ratio_height", 1);
    }

    Logger::log_info("sent aspect ratio");
    return true;
}

// Add aspect ratio properties to the properties list.
static void add_aspect_ratio_properties(obs_properties_t* props, void* data) {
    // Standard aspect ratios.
    obs_properties_add_button(props, "button_16_9", "16:9", [](obs_properties_t* props, obs_property_t* property, void* data) {
        return aspect_ratio_button_clicked(props, property, data, 16, 9);
        });
    obs_properties_add_button(props, "button_9_16", "9:16", [](obs_properties_t* props, obs_property_t* property, void* data) {
        return aspect_ratio_button_clicked(props, property, data, 9, 16);
        });
    obs_properties_add_button(props, "button_4_3", "4:3", [](obs_properties_t* props, obs_property_t* property, void* data) {
        return aspect_ratio_button_clicked(props, property, data, 4, 3);
        });
    obs_properties_add_button(props, "button_3_4", "3:4", [](obs_properties_t* props, obs_property_t* property, void* data) {
        return aspect_ratio_button_clicked(props, property, data, 3, 4);
        });
    obs_properties_add_button(props, "button_default", "default", [](obs_properties_t* props, obs_property_t* property, void* data) {
        return aspect_ratio_button_clicked(props, property, data, -1, -1);
        });

    // Add an integer slider for width
    obs_properties_add_int_slider(props, "aspect_ratio_width", "Custom Aspect Ratio Width", 1, 100, 1);

    // Add an integer slider for height
    obs_properties_add_int_slider(props, "aspect_ratio_height", "Custom Aspect Ratio Height", 1, 100, 1);


}
static void custom_get_defaults(obs_data_t* settings) {
    Logger::log_info("get defaults called");
    obs_data_set_default_int(settings, "aspect_ratio_width", 50); // Default width
    obs_data_set_default_int(settings, "aspect_ratio_height", 50); // Default height
}

static obs_properties_t* custom_get_properties(void* data) {
    Logger::log_info("custom properties called");
    blog(LOG_INFO, "custom properties called");
    obs_properties_t* props = obs_properties_create();
    blog(LOG_INFO, "custom properties created");
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);

    obs_property_t* p = obs_properties_add_list(props, "ndi_source_list", "NDI Sources", OBS_COMBO_TYPE_LIST, OBS_COMBO_FORMAT_STRING);

    for (const auto& source_name : custom->ndiReceiver->getCurrentSources()) {
        blog(LOG_INFO, (std::string("adding source: ") + source_name).c_str());
        obs_property_list_add_string(p, source_name.c_str(), source_name.c_str());
    }
    obs_properties_add_float_slider(props, "zoom_slider", "Zoom", 0.0, 1.0, 0.01);
    obs_property_t* switch_camera_button = obs_properties_add_button(props, "switch_camera", "Switch Camera", switch_camera_button_clicked);
    add_aspect_ratio_properties(props, data);
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
        gs_matrix_push();

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
    std::string selectedNdiSource = obs_data_get_string(settings, "ndi_source_list");
    if (selectedNdiSource != custom->selected_ndi_source) {
        custom->selected_ndi_source = obs_data_get_string(settings, "ndi_source_list");
        if (custom->selected_ndi_source == "TEST") {
            custom->text_pos.x = custom->width / 2.0;
            custom->text_pos.y = custom->height / 2.0;
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
    float zoom_value = obs_data_get_double(settings, "zoom_slider");
    if (zoom_value != custom->zoom) {
        custom->zoom = zoom_value;
        custom->ndiReceiver->sendMetadata(Zoom{ zoom_value });
    }
    int aspectRatioWidth = obs_data_get_int(settings, "aspect_ratio_width");
    int aspectRatioHeight = obs_data_get_int(settings, "aspect_ratio_height");

    if (aspectRatioWidth != custom->aspect_ratio_width) {
        custom->aspect_ratio_width = aspectRatioWidth;
        AspectRatio as = { custom->aspect_ratio_width, custom->aspect_ratio_height };
        custom->ndiReceiver->sendMetadata(as);
    }
    if (aspectRatioHeight != custom->aspect_ratio_height) {
        custom->aspect_ratio_height = aspectRatioHeight;
        AspectRatio as = { custom->aspect_ratio_width, custom->aspect_ratio_height };
        custom->ndiReceiver->sendMetadata(as);
    }
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
    data->source = source;
    // Initialize text velocity (you can adjust these values)
    data->text_vel.x = -5.0 * 2;
    data->text_vel.y = 8.6 * 2;
    data->obs_source_name = obs_source_get_name(source);
    std::lock_guard<std::mutex> lock(customDataMutex);
    sharedCustomDataVector.push_back(data);
    blog(LOG_INFO, "going to call update");
    custom_update(data, settings);
    blog(LOG_INFO, "done");
    data->ndiReceiver->addAudioCallback([data](common_types::Audio audio) {

        struct obs_source_audio obs_audio_frame;
        memset(&obs_audio_frame, 0, sizeof(obs_audio_frame));
        std::vector<float> vec(audio.noSamples * audio.channels, 0);
        // Assign the deinterleaved buffers to the OBS audio frame
        obs_audio_frame.data[0] = reinterpret_cast<const uint8_t*>(audio.data.data());

        obs_audio_frame.frames = audio.noSamples;
        obs_audio_frame.speakers = SPEAKERS_STEREO; // Adjust based on actual channel count
        obs_audio_frame.samples_per_sec = audio.sampleRate;
        obs_audio_frame.format = AUDIO_FORMAT_FLOAT;
        obs_audio_frame.timestamp = audio.timestamp * 100;
        // Output audio to OBS
        auto obs_source_name = obs_source_get_name(data->source);

        obs_source_output_audio(data->source, &obs_audio_frame);
        });
    /*data->ndiReceiver->addFrameCallback([data](common_types::Image image) {
        struct obs_source_frame obs_frame;
        memset(&obs_frame, 0, sizeof(struct obs_source_frame));

        // Assuming the image format is RGBA for this example, modify as needed
        obs_frame.format = VIDEO_FORMAT_RGBA;
        obs_frame.width = image.width;
        obs_frame.height = image.height;
        obs_frame.timestamp = image.timestamp;
        // Assuming the data is tightly packed, modify if stride (bytes per line) is different
        obs_frame.linesize[0] = image.width * 4; // RGBA is 4 bytes per pixel

        // You need to ensure that the data lives as long as OBS needs it to,
        // which may involve copying it to a persistent buffer or ensuring
        // your image object lives long enough.
        obs_frame.data[0] = image.data.data();

        // Flip the image vertically if necessary
        obs_frame.flip = false; // Set to true if the image is upside down
        
        // Output the frame to OBS
        obs_source_output_video(data->source, &obs_frame);
        data->height = image.height;
        data->width = image.width;
        });*/
    data->ndiReceiver->setVideoConnectedCallback([data](Image img) {
        obs_source_set_enabled(data->source, true);
        });
    data->ndiReceiver->setVideoDisconnectedCallback([data]() {
        obs_source_set_enabled(data->source, false);
        });
    return data;
}

static void custom_destroy(void* data) {
    Logger::log_info("destroy called");
    struct custom_data* custom = reinterpret_cast<custom_data*>(data);
    Logger::log_info("going to delete custom");
    custom->ndiReceiver->stop();
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
    .output_flags = OBS_SOURCE_VIDEO | OBS_SOURCE_AUDIO | OBS_SOURCE_DO_NOT_DUPLICATE | OBS_SOURCE_CAP_DONT_SHOW_PROPERTIES,
    .get_name = [](void*) -> const char* { return "Siili Cam Source"; },
    .create = custom_create,
    .destroy = custom_destroy,
    .get_width = custom_get_width,       // Add this line
    .get_height = custom_get_height,
    .get_defaults = custom_get_defaults,
    .get_properties = custom_get_properties,
    .update = custom_update,
    .video_render = custom_video_render,
    // ... [Other necessary callbacks and fields]
};
