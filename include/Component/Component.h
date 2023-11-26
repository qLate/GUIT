#pragma once

#include <string>
#include <vector>

#include "wayland-client.h"
#include "Color.h"
#include "vec2.hpp"
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>

class SubComponent;
class WindowW;

class Component
{
protected:
	wl_callback_listener callbackListener {
		.done = frameNew
	};
	wl_surface_listener surfListener = {
		.enter = onSurfaceEnterCallback,
		.leave = onSurfaceLeaveCallback,
	};

	glm::ivec2 imageSize {1, 1};
	std::vector<uint8_t> imageData {255, 255, 255, 255};
	std::vector<uint8_t> scaledImageData {255, 255, 255, 255};
	cairo_surface_t* imageDataSurf;

public:
	std::vector<SubComponent*> subComponents {};

	wl_surface* surf = nullptr;
	wl_callback* surfCallback = nullptr;

	wl_egl_window* eWindow = nullptr;
	EGLSurface eSurf = nullptr;
	cairo_surface_t* cSurf = nullptr;

	glm::vec2 size;
	int x = 0, y = 0;

	WindowW* window;
	bool moveWindowOnDrag;


	Component(glm::vec2 size);
	virtual ~Component();

	virtual void update();

	virtual void resize(glm::vec2 size);

	void updateImageData();
	void setColor(Color color);
	void setImage(const std::string& path);

	virtual void onSurfaceEnter(wl_surface* surface, wl_output* output) {}
	virtual void onSurfaceLeave(wl_surface* surface, wl_output* output) {}

	static void frameNew(void* data, wl_callback* cb, uint32_t a);
	static void onSurfaceEnterCallback(void* data, wl_surface* surface, wl_output* output);
	static void onSurfaceLeaveCallback(void* data, wl_surface* surface, wl_output* output);

	friend class GUIToolkitListeners;
	friend class Utils;
};
