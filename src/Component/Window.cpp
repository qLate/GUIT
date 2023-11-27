#include "Window.h"

#include <cstring>

#include "Debug.h"
#include "GUI.h"
#include "Utils.h"

WindoW::WindoW(const std::string& name, glm::vec2 size): Component(size)
{
	this->window = this;

	wSurf = wl_compositor_create_surface(GUI::compositor);
	wl_surface_set_user_data(wSurf, this);

	wSurfCallback = wl_surface_frame(wSurf);
	wl_callback_add_listener(wSurfCallback, &wCallbackListener, this);
	wl_surface_commit(wSurf);

	xSurf = xdg_wm_base_get_xdg_surface(GUI::sharedBase, wSurf);
	xdg_surface_add_listener(xSurf, &xSurfListener, this);

	top = xdg_surface_get_toplevel(xSurf);
	xdg_toplevel_add_listener(top, &topListener, this);
	xdg_toplevel_set_title(top, name.data());

	subsurf = wl_subcompositor_get_subsurface(GUI::subcompositor, surf, wSurf);

	resize(size);
	GUI::windows.push_back(this);
}
WindoW::~WindoW()
{
	std::erase(GUI::windows, this);

	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xSurf);
}
void WindoW::resize(glm::vec2 size)
{
	if (this->size == size) return;

	int w = (int)size.x + (isFullscreen ? 0 : 2 * resizeBorder);
	int h = (int)size.y + (isFullscreen ? 0 : 2 * resizeBorder + headerHeight);
	Utils::resizeSurface(this->wSize, {w, h}, wPixelsCapacity, wSurf, wBuf, wPixels, wPixels_pool);
	wl_subsurface_set_position(subsurf, isFullscreen ? 0 : resizeBorder, isFullscreen ? 0 : resizeBorder + headerHeight);

	this->wSize = {w, h};

	Component::resize(size);
}
void WindoW::draw()
{
	Component::draw();

	drawHeader();
}
void WindoW::drawHeader() const
{
	Debug::funcEntered(__FUNCTION__);
	memset(wPixels, 0, wSize.x * wSize.y * 4);
	for (int y = 0; y < headerHeight; y++)
	{
		for (int x = 0; x < size.x; x++)
		{
			int i = (y + resizeBorder) * (int)wSize.x + x + resizeBorder;
			memset(wPixels + 4 * i, 255, 4);
		}
	}
	wl_surface_attach(wSurf, wBuf, 0, 0);
	wl_surface_damage_buffer(wSurf, 0, 0, wSize.x, resizeBorder + headerHeight + 1);
	wl_surface_commit(wSurf);
	Debug::funcExit(__FUNCTION__);
}

void WindoW::switchFullscreen()
{
	isFullscreen = !isFullscreen;

	if (window->isFullscreen)
		xdg_toplevel_set_fullscreen(window->top, nullptr);
	else
		xdg_toplevel_unset_fullscreen(window->top);
}

void WindoW::frameNew(void* data, wl_callback* cb, uint32_t a)
{
	Debug::funcEntered(__FUNCTION__);
	auto window = (WindoW*)data;

	wl_callback_destroy(cb);
	window->wSurfCallback = wl_surface_frame(window->wSurf);
	wl_callback_add_listener(window->wSurfCallback, &window->wCallbackListener, window);
	wl_surface_commit(window->wSurf);

	window->draw();
	Debug::funcExit(__FUNCTION__);
}
void WindoW::configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial)
{
	Debug::funcEntered(__FUNCTION__);
	auto window = (WindoW*)data;
	xdg_surface_ack_configure(xSurf, serial);

	window->resize(window->size + (window->isFullscreen ? glm::vec2(resizeBorder * 2, resizeBorder * 2 + headerHeight) : glm::vec2()));
	Debug::funcExit(__FUNCTION__);
}
void WindoW::configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat)
{
	Debug::funcEntered(__FUNCTION__, w_, " ", h_);
	if (w_ == 0 || h_ == 0)
	{
		Debug::funcExit(__FUNCTION__);
		return;
	}

	w_ = std::max(w_, minSize + 2 * resizeBorder);
	h_ = std::max(h_, minSize + 2 * resizeBorder);

	auto window = (WindoW*)data;
	if (window->size.x != w_ || window->size.y != h_)
		window->resize(glm::vec2(w_, h_) - glm::vec2(2 * resizeBorder, 2 * resizeBorder + headerHeight));
	Debug::funcExit(__FUNCTION__);
}
void WindoW::closeTop(void* data, xdg_toplevel* top) {}
