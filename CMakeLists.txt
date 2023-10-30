cmake_minimum_required (VERSION 3.8)
project ("GUIToolkit")
set(CMAKE_CXX_STANDARD 20)

set(SOURCE_FILES "src/Component/Component.cpp" "src/xdg-shell-protocol.c"  "src/main.cpp" "src/GUIToolkit.cpp" "src/GUIToolkitListeners.cpp"  "include/Action.h" "src/Component/Window.cpp" "src/Component/SubComponent.cpp"  "src/Utils.cpp")

include_directories(libs/glm libs/glm/glm include include/Component libs)

add_executable (GUIT ${SOURCE_FILES})

# Find wayland package
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Wayland REQUIRED wayland::client wayland::cursor)
include_directories(${Wayland_INCLUDE_DIR})
target_link_libraries(GUIT ${Wayland_LIBRARIES} wayland::client wayland::cursor)
