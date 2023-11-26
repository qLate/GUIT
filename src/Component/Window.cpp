#include "Window.h"

#include "Debug.h"
#include "GUIToolkit.h"
#include "Utils.h"

WindowW::WindowW(const std::string& name, glm::vec2 size): Component(size)
{
	this->window = this;
	this->moveWindowOnDrag = true;

	wSurf = wl_compositor_create_surface(GUIToolkit::compositor);
	wl_surface_set_user_data(wSurf, this);
	wl_surface_add_listener(wSurf, &wSurfListener, this);

	wSurfCallback = wl_surface_frame(wSurf);
	wl_callback_add_listener(wSurfCallback, &wCallbackListener, this);
	wl_surface_commit(wSurf);

	xSurf = xdg_wm_base_get_xdg_surface(GUIToolkit::sharedBase, wSurf);
	xdg_surface_add_listener(xSurf, &xSurfListener, this);

	top = xdg_surface_get_toplevel(xSurf);
	xdg_toplevel_add_listener(top, &topListener, this);
	xdg_toplevel_set_title(top, name.data());

	subsurf = wl_subcompositor_get_subsurface(GUIToolkit::subcompositor, surf, wSurf);

	resize(size);
	GUIToolkit::windows.push_back(this);
}
WindowW::~WindowW()
{
	std::erase(GUIToolkit::windows, this);

	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xSurf);
}
void WindowW::resize(glm::vec2 size)
{
	if(this->size == size) return;
	int border = GUIToolkit::windowResizeBorder;

	int w = (int)size.x + (isFullscreen ? 0 : 2 * border);
	int h = (int)size.y + (isFullscreen ? 0 : 2 * border);
	Utils::resizeSurface(this->wSize, {w, h}, wPixelsCapacity, wSurf, wBuf, wPixels, wPixels_pool);
	wl_subsurface_set_position(subsurf, isFullscreen ? 0 : border, isFullscreen ? 0 : border);

	this->wSize = {w, h};

	Component::resize(size);
}
void WindowW::update() 
{
	Component::update();

	wl_surface_damage_buffer(wSurf, 0, 0, size.x + 2 * GUIToolkit::windowResizeBorder, size.y + 2 * GUIToolkit::windowResizeBorder);
	wl_surface_commit(wSurf);
}
void WindowW::switchFullscreen()
{
	isFullscreen = !isFullscreen;

	if (window->isFullscreen)
		xdg_toplevel_set_fullscreen(window->top, nullptr);
	else
		xdg_toplevel_unset_fullscreen(window->top);
}

void WindowW::configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial)
{
	Debug::funcEntered(__FUNCTION__);
	auto window = (WindowW*)data;
	xdg_surface_ack_configure(xSurf, serial);

	window->resize(window->size + (window->isFullscreen ? glm::vec2(GUIToolkit::windowResizeBorder * 2, GUIToolkit::windowResizeBorder * 2) : glm::vec2()));
	Debug::funcExit(__FUNCTION__);
}
void WindowW::configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat)
{
	Debug::funcEntered(__FUNCTION__);
	if (w_ <= 0 || h_ <= 0)
	{
		Debug::funcExit(__FUNCTION__);
		return;
	}

	auto window = (WindowW*)data;
	if (window->size.x != w_ || window->size.y != h_)
		window->resize(glm::vec2(w_, h_) - glm::vec2(2 * GUIToolkit::windowResizeBorder, 2 * GUIToolkit::windowResizeBorder));
	Debug::funcExit(__FUNCTION__);
}
void WindowW::closeTop(void* data, xdg_toplevel* top) {}
