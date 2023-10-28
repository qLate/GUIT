#include "Window.h"
#include "GUIToolkit.h"

Window::Window(const std::string& name, glm::vec2 size) : Component(size)
{
	this->window = this;
	this->isWindowMoveable = true;

	xSurf = xdg_wm_base_get_xdg_surface(GUIToolkit::instance->sharedBase, surf);
	xdg_surface_add_listener(xSurf, &xSurfListener, this);

	top = xdg_surface_get_toplevel(xSurf);
	xdg_toplevel_add_listener(top, &topListener, this);
	xdg_toplevel_set_title(top, name.data());

	GUIToolkit::windows.push_back(this);
}
Window::~Window()
{
	std::erase(GUIToolkit::windows, this);
}
void Window::destroy()
{
	Component::destroy();

	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xSurf);
}


void Window::configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial)
{
	auto window = (Window*)data;
	xdg_surface_ack_configure(xSurf, serial);

	window->update();
}
void Window::configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat)
{
	if (!w_ && !h_) return;

	auto window = (Window*)data;
	if ((int)window->size.x != w_ || (int)window->size.y != h_)
		window->resize({w_, h_});
}
void Window::closeTop(void* data, xdg_toplevel* top)
{
	auto window = (Window*)data;
	window->destroy();
}

void Window::onSurfaceEnter(wl_surface* surface, wl_output* output)
{
	Component::onSurfaceEnter(surface, output);

	GUIToolkit::focusedComponent = this;
}
void Window::onSurfaceLeave(wl_surface* surface, wl_output* output)
{
	Component::onSurfaceLeave(surface, output);

	GUIToolkit::focusedComponent = nullptr;
}
