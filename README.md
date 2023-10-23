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

## Installation

1. Download the plugin from the Releases page.
2. Follow the installation guide to add the plugin to OBS.

## How To Use

1. Open OBS and go to the "Tools" menu.
2. Select the "SiiliCam-OBS" option.
3. Add your SiiliCam sources.
4. Configure and control them as you wish.

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
