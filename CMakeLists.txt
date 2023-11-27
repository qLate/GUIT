cmake_minimum_required (VERSION 3.8)
project ("GUIToolkit")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(SOURCE_FILES "src/Component/Component.cpp" "src/xdg-shell-protocol.c" "src/main.cpp" "src/GUI.cpp" "src/GUIListeners.cpp"
				 "include/Action.h" "src/Component/Window.cpp" "src/Component/SubComponent.cpp" "src/Utils.cpp"
		include/Component/TextComponent.h "include/Debug.h")

include_directories(libs/glm libs/glm/glm include include/Component libs)

add_executable (GUIT ${SOURCE_FILES})

# Find wayland package
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}")
find_package(Wayland)
include_directories(${Wayland_INCLUDE_DIR})
target_link_libraries(GUIT ${WAYLAND_LIBRARIES})

find_package(Cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
target_link_libraries(GUIT ${CAIRO_LIBRARIES})

target_link_libraries(GUIT EGL)