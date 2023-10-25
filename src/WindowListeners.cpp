#include "WindowListeners.h"

#include "GUIToolkit.h"
#include "Window.h"


void WindowListeners::configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial)
{
	xdg_surface_ack_configure(xSurf, serial);

	auto window = (Window*)data;
	window->draw();
}

void WindowListeners::configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat)
{
	if (!w_ && !h_) return;

	auto window = (Window*)data;
	if (window->w != w_ || window->h != h_)
		window->resize(w_, h_);
}

void WindowListeners::closeTop(void* data, xdg_toplevel* top)
{
	auto window = (Window*)data;
	window->wClose();
}

void WindowListeners::frameNew(void* data, wl_callback* cb, uint32_t a)
{
	auto window = (Window*)data;

	wl_callback_destroy(cb);
	//if (window->isClosed) return;
	cb = wl_surface_frame(window->surf);
	wl_callback_add_listener(cb, &window->listeners.callbackListener, window);

	window->draw();
}

void WindowListeners::onSurfaceEnter(void* data, wl_surface* surface, wl_output* output)
{
	auto window = (Window*)data;
	GUIToolkit::activeWindow = window;
}
void WindowListeners::onSurfaceLeave(void* data, wl_surface* surface, wl_output* output)
{
	GUIToolkit::activeWindow = nullptr;
}
