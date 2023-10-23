# CMakeList.txt : CMake project for SiiliCam-obs, include source and define
# project specific logic here.
#

# Add source to this project's executable.
set(OBS_INCLUDE_PATH "C:/Users/Simo/source/repos/obs-studio")
set(OBS_LIB_PATH "C:/Users/Simo/source/repos/obs-studio/build_x64/libobs/Release")
set(OBS_FRONTEND_LIB_PATH "C:/Users/Simo/source/repos/obs-studio/build_x64/UI/obs-frontend-api/Release")
set(OBS_GRAPHICS_LIB_PATH "C:/Users/Simo/source/repos/obs-studio/build_x64/UI/obs-frontend-api/Release")


add_library (SiiliCam-obs MODULE "siilicam-source.cpp" "SiiliCam-obs.cpp"  "include/RestHandlers.h")
target_include_directories(SiiliCam-obs PRIVATE 
    "${OBS_INCLUDE_PATH}/libobs"
    "${OBS_INCLUDE_PATH}/UI/obs-frontend-api"
    "${OBS_INCLUDE_PATH}/libobs/graphics"
    include
)

target_link_libraries(SiiliCam-obs PRIVATE 
    "${OBS_LIB_PATH}/obs.lib"
    "${OBS_FRONTEND_LIB_PATH}/obs-frontend-api.lib"
    # Add other necessary libraries here
    RestServer
    Boost::system
    Boost::json
    ${CMAKE_THREAD_LIBS_INIT}
    Logger
    NDIWrapper
)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SiiliCam-obs PROPERTY CXX_STANDARD 20)
endif()

add_custom_command(TARGET SiiliCam-obs POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy_if_different
  "${NDI_SDK_PATH}/Bin/x64/Processing.NDI.Lib.x64.dll"
  $<TARGET_FILE_DIR:SiiliCam-obs>
)
add_dependencies(SiiliCam-obs Logger RestServer NDIWrapper)
# TODO: Add tests and install targets if needed.
