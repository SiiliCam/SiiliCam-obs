#pragma once

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include "shared-data.h"
#include "RestServer.hpp"


Response getSources(Request req) {
	boost::property_tree::ptree root;

	// Step 2: Retrieve All Sources
	{
		std::lock_guard<std::mutex> lock(customDataMutex);

		boost::property_tree::ptree sourcesArray;
		for (auto& data : sharedCustomDataVector) {
			boost::property_tree::ptree sourceNode;
			data->obs_source_name = obs_source_get_name(data->source);
			sourceNode.put("obs_source_name", data->obs_source_name);
			sourceNode.put("selected_ndi_source", data->selected_ndi_source);

			sourcesArray.push_back(std::make_pair("", sourceNode));
		}
		root.add_child("sources", sourcesArray);
	}

	// Step 3: Retrieve All NDI Sources
	if (!sharedCustomDataVector.empty()) {
		std::vector<std::string> availableNdiSources = sharedCustomDataVector[0]->ndiReceiver->getCurrentSources();

		boost::property_tree::ptree ndiSourcesArray;
		for (const auto& src : availableNdiSources) {
			boost::property_tree::ptree ndiNode;
			ndiNode.put("", src);
			ndiSourcesArray.push_back(std::make_pair("", ndiNode));
		}

		root.add_child("available_ndi_sources", ndiSourcesArray);
	}

	return Response{ root, 200 };
	};
Response setSource(Request req) {
	Logger::log_info("set ndi source called");
	std::string requestBody = req.body();
	boost::property_tree::ptree pt;
	std::stringstream ss(requestBody);
	boost::property_tree::read_json(ss, pt);
	std::string obs_source_name = pt.get<std::string>("obs_source_name");
	std::string ndi_source = pt.get<std::string>("ndi_source");

	// Check if OBS source exists
	custom_data* foundCustomData = nullptr;
	{
		std::lock_guard<std::mutex> lock(customDataMutex);
		for (auto& data : sharedCustomDataVector) {
			if (data->obs_source_name == obs_source_name) {
				foundCustomData = data;
				break;
			}
		}
	}

	if (!foundCustomData) {
		return Response{ "OBS source not found", 404 };
	}

	// Set NDI source and update custom data
	if (foundCustomData->ndiReceiver->setOutput(ndi_source)) {
		foundCustomData->selected_ndi_source = ndi_source;
		return Response{ "Source updated successfully", 200 };
	}
	else {
		return Response{ "Failed to set NDI source", 500 };  // Internal Server Error
	}
}

Response setFirstMatchingNdiSource(Request req) {
	Logger::log_info("set first matching NDI source called");
	std::string requestBody = req.body();
	boost::property_tree::ptree pt;
	std::stringstream ss(requestBody);
	boost::property_tree::read_json(ss, pt);
	std::string obs_source_name = pt.get<std::string>("obs_source_name");
	std::string ndi_source_substring = pt.get<std::string>("ndi_source");

	// Check if OBS source exists
	custom_data* foundCustomData = nullptr;
	{
		std::lock_guard<std::mutex> lock(customDataMutex);
		for (auto& data : sharedCustomDataVector) {
			if (data->obs_source_name == obs_source_name) {
				foundCustomData = data;
				break;
			}
		}
	}

	if (!foundCustomData) {
		return Response{ "OBS source not found", 404 };
	}

	// Find the first matching NDI source
	std::vector<std::string> availableNdiSources = foundCustomData->ndiReceiver->getCurrentSources();
	std::string matchedNdiSource;
	for (const auto& src : availableNdiSources) {
		if (src.find(ndi_source_substring) != std::string::npos) {
			matchedNdiSource = src;
			break;
		}
	}

	if (matchedNdiSource.empty()) {
		return Response{ "Matching NDI source not found", 404 };
	}

	// Set the found NDI source and update custom data
	if (foundCustomData->ndiReceiver->setOutput(matchedNdiSource)) {
		foundCustomData->selected_ndi_source = matchedNdiSource;
		return Response{ "Source updated successfully", 200 };
	}
	else {
		return Response{ "Failed to set NDI source", 500 };  // Internal Server Error
	}
}
void updateTextSource(const std::string& sourceName, const std::string& newText) {
	obs_source_t* source = obs_get_source_by_name(sourceName.c_str());
	if (source) {
		const char* source_id = obs_source_get_id(source);

		// Check against the updated source ID
		if (strcmp(source_id, "text_gdiplus") == 0 || strcmp(source_id, "text_ft2_source") == 0 || strcmp(source_id, "text_gdiplus_v2") == 0) {
			obs_data_t* settings = obs_source_get_settings(source);
			obs_data_set_string(settings, "text", newText.c_str());
			obs_source_update(source, settings);
			obs_data_release(settings);
		}
		else {
			Logger::log_error("Source not a valid text type: ", sourceName, ", ID: ", source_id);
		}
		obs_source_release(source);
	}
	else {
		Logger::log_error("Text source not found: ", sourceName);
	}
}


Response setCameraVisibility(Request req) {
	Logger::log_info("set camera visibility called");
	std::string requestBody = req.body();
	boost::property_tree::ptree pt;
	std::stringstream ss(requestBody);
	boost::property_tree::read_json(ss, pt);
	std::string obs_source_name = pt.get<std::string>("obs_source_name");
	bool visible = pt.get<bool>("visibility");

	// Check if OBS source exists
	custom_data* foundCustomData = nullptr;
	{
		std::lock_guard<std::mutex> lock(customDataMutex);
		for (auto& data : sharedCustomDataVector) {
			if (data->obs_source_name == obs_source_name) {
				foundCustomData = data;
				break;
			}
		}
	}

	if (!foundCustomData) {
		return Response{ "OBS source not found", 404 };
	}

	Logger::log_info("visibility is:", std::to_string(visible));
	// Set the visibility of the OBS source
	obs_source_set_enabled(foundCustomData->source, visible);

	return Response{ visible ? "Source shown successfully" : "Source hidden successfully", 200 };
}

Response setTextSource(Request req) {
	Logger::log_info("setTextSource called");

	std::string requestBody = req.body();
	boost::property_tree::ptree pt;
	std::stringstream ss(requestBody);
	boost::property_tree::read_json(ss, pt);

	std::string textSourceName = pt.get<std::string>("textSourceName");
	std::string newText = pt.get<std::string>("text");

	updateTextSource(textSourceName, newText);

	return Response{ "Text updated successfully", 200 };
}