# SiiliCam-OBS

SiiliCam-OBS is a Windows OBS plugin tailored specifically for seamless integration with [SiiliCam](https://github.com/SiiliCam). 
SiiliCam sources are essentially phones running the SiiliCam application, transmitting video data over NDI. **Note: The current version of this plugin only supports SiiliCam sources and not generic NDI sources.**

With this plugin, you can directly display this NDI data in OBS by simply selecting the SiiliCam source from the list of available sources.
Additionally, you can manipulate the SiiliCam source using options like zoom and camera switch within OBS.
The plugin also runs a REST server, allowing you to control it via HTTP requests.

## Features

- **SiiliCam Specialization**: Designed to exclusively work with SiiliCam sources (**does not support generic NDI sources**).
- **REST Server**: Control the SiiliCam-OBS source using HTTP requests.
- **View SiiliCam Sources**: Display video data from phones running the SiiliCam application directly in OBS.
- **Manipulate SiiliCam Sources**: Zoom, switch camera, and other functionalities are available and will be remembered for future sessions.
  
## Requirements

- Windows operating system
- OBS Studio

## How To Use

1. Select the "SiiliCam source" in sources.
2. Configure and control them as you wish.

## API

You can control SiiliCam-OBS via REST API for easy programmability and integration into other systems.

## Under the Hood

The plugin is developed using C++, and it utilizes the following:

- OBS Source API for rendering.
- NDI SDK for NDI functionalities.
- Custom properties to allow users to select from available NDI sources.

SiiliCam-OBS uses [NDIWrapper](https://github.com/SiiliCam/NDIWrapper) for NDI functionalities. To know more about NDIWrapper, [click here](https://github.com/SiiliCam/NDIWrapper).

## Contributing and Support

Feel free to open issues or pull requests if you have suggestions or run into any issues.

---

## SiiliCam-OBS REST API Documentation

---

### Table of Contents

1. [Introduction](#introduction)
2. [Get Sources](#get-sources)
3. [Set NDI Source](#set-ndi-source)

---

### Introduction

This REST API is a part of the SiiliCam-OBS plugin and allows you to get information about available sources and set NDI sources programmatically.

---

### Get Sources

- **Endpoint**: `GET localhost:6042/getSources`
- **Description**: Fetches information about available OBS sources and their selected NDI sources.
- **Response Format**: JSON
- **Response Codes**: 200 OK

#### Example Response

```json
{
    "sources": [
        {
            "obs_source_name": "Siili Cam Source",
            "selected_ndi_source": "LOCALHOST (SIILISM-G991B1)"
        },
        {
            "obs_source_name": "Siili Cam Source 2",
            "selected_ndi_source": "LOCALHOST (SIILISM-G970F1)"
        }
    ],
    "available_ndi_sources": [
        "LOCALHOST (SIILISM-G970F1)",
        "LOCALHOST (SIILISM-G991B1)"
    ]
}
```

---

### Set NDI Source

- **Endpoint**: `POST localhost:6042/setNDISource`
- **Description**: Sets an NDI source for an existing OBS source.
- **Request Body**: JSON format
- **Response Format**: JSON
- **Response Codes**: 
  - 200 OK
  - 404 Not Found (OBS source not found)
  - 500 Internal Server Error (Failed to set NDI source)

#### Example Request

```json
{
    "obs_source_name": "Siili Cam Source 2",
    "ndi_source": "LOCALHOST (SIILISM-G970F1)"
}
```

#### Example Responses

**Successful Update**

```json
{
    "message": "Source updated successfully"
}
```

**Failed Update**

```json
{
    "message": "Failed to set NDI source"
}
```

**OBS Source Not Found**

```json
{
    "message": "OBS source not found"
}
```

**Note**: If the source is trying to be set and it's non-existent, the receiver will wait until it comes up.

