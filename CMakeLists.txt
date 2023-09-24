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
set(CMAKE_SYSTEM_VERSION 10.0.20348.0 CACHE TYPE INTERNAL FORCE)

project ("SiiliCam-obs")
add_subdirectory("external/Logger")
add_subdirectory("external/CommonTypes")
add_subdirectory("external/NDIWrapper")
add_custom_target(
    BuildObsStudio
    COMMAND cmake --build . --preset windows-x64
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/external/obs-studio
)

# Include sub-projects.
#add_subdirectory ("SiiliCam-obs")
