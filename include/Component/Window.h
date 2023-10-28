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

	xdg_toplevel* top = nullptr;
	xdg_surface* xSurf = nullptr;

public:
	Window(const std::string& name, glm::vec2 size);
	~Window() override;

	void destroy() override;

	static void configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial);
	static void configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat);
	static void closeTop(void* data, xdg_toplevel* top);
	void onSurfaceEnter(wl_surface* surface, wl_output* output) override;
	void onSurfaceLeave(wl_surface* surface, wl_output* output) override;

	friend class GUIToolkitListeners;
};
