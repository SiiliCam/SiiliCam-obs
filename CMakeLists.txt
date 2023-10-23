# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

set(BUILD_ONLY_LIB ON)

set(NDI_SDK_PATH "C:/Program Files/NDI/NDI 5 SDK")

project ("SiiliCam-obs" VERSION 1.0.0 LANGUAGES C CXX)

set(Boost_DIR "C:/local/boost_1_83_0/stage/lib/cmake/Boost-1.83.0/")
SET(Boost_ADDITIONAL_VERSIONS 1.83.0)
find_package(Boost 1.83.0 REQUIRED COMPONENTS system json log log_setup)

add_subdirectory("external/Logger")
add_subdirectory("external/CommonTypes")
add_subdirectory("external/NDIWrapper")
add_subdirectory("external/RestServer")

find_package(Threads REQUIRED)

# Include sub-projects.
add_subdirectory ("SiiliCam-obs")
