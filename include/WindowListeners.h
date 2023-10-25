#pragma once
#include "xdg-shell-client-protocol.h"


class WindowListeners
{
	xdg_surface_listener xSurfListener {
		.configure = configureXSurf
	};
	xdg_toplevel_listener topListener {
		.configure = configureTop,
		.close = closeTop
	};
	wl_callback_listener callbackListener {
		.done = frameNew
	};
	wl_surface_listener surface_listener = {
		.enter = onSurfaceEnter,
		.leave = onSurfaceLeave,
	};

	static void configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial);
	static void configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat);
	static void closeTop(void* data, xdg_toplevel* top);
	static void frameNew(void* data, wl_callback* cb, uint32_t a);

	static void onSurfaceEnter(void* data, wl_surface* surface, wl_output* output);
	static void onSurfaceLeave(void* data, wl_surface* surface, wl_output* output);

	friend class Window;
};
