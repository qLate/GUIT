cmake_minimum_required (VERSION 3.8)
project ("GUIToolkit")
set(CMAKE_CXX_STANDARD 14)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Wayland REQUIRED wayland::client)

add_executable (GUIT "w.c" "xdg-shell-protocol.c")

include_directories(${Wayland_INCLUDE_DIR})
target_link_libraries(GUIT ${Wayland_LIBRARIES} wayland::client)
