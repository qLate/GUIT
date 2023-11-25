#pragma once
#include <string>

#include "Component.h"
#include "xdg-shell-client-protocol.h"

class Window : public Component
{
	xdg_surface_listener xSurfListener {
		.configure = configureXSurf
	};
	xdg_toplevel_listener topListener {
		.configure = configureTop,
		.close = closeTop
	};

	wl_callback_listener wCallbackListener {
		.done = frameNew
	};
	wl_surface_listener wSurfListener = {
		.enter = onSurfaceEnterCallback,
		.leave = onSurfaceLeaveCallback,
	};

	xdg_toplevel* top = nullptr;
	xdg_surface* xSurf = nullptr;

	glm::vec2 wSize;
	wl_surface* wSurf = nullptr;
	wl_callback* wSurfCallback = nullptr;
	wl_buffer* wBuf = nullptr;
	uint8_t* wPixels = nullptr;
	wl_shm_pool* wPixels_pool = nullptr;
	int wPixelsCapacity = 0;

	wl_subsurface* subsurf = nullptr;

	bool isFullscreen = false;

public:
	bool isMoveable = true;
	bool isResizeable = true;

	Window(const std::string& name, glm::vec2 size);
	~Window() override;

	void resize(glm::vec2 size) final;
	void update() const override;

	void switchFullscreen();

	static void configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial);
	static void configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat);
	static void closeTop(void* data, xdg_toplevel* top);

	friend class GUIToolkitListeners;
};
