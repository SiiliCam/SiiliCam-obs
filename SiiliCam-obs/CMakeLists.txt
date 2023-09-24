# CMakeList.txt : CMake project for SiiliCam-obs, include source and define
# project specific logic here.
#

# Add source to this project's executable.
add_executable (SiiliCam-obs "SiiliCam-obs.cpp" "SiiliCam-obs.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET SiiliCam-obs PROPERTY CXX_STANDARD 20)
endif()

# TODO: Add tests and install targets if needed.
