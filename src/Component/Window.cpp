#include "Window.h"

#include <cstring>

#include "Debug.h"
#include "GUI.h"
#include "InputField.h"
#include "SubComponent.h"
#include "Utils.h"

WindoW::WindoW(const std::string& name, glm::vec2 size): Component(size)
{
	GUI::windows.push_back(this);
	this->window = this;

	initSurfaces(name);

	resize(size);
	createHeader(name);

	//draw();
}
void WindoW::initSurfaces(const std::string& name)
{
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
	//wl_subsurface_set_desync(subsurf);
}
void WindoW::createHeader(const std::string& name)
{
	header = new SubComponent(this, {size.x, headerHeight}, wSurf);
	header->nameID = "header";
	header->setPivot({0, 0});
	header->setLocalPos({resizeBorder, resizeBorder});
	header->setAnchors({0, 0}, {1, 0});
	header->setColor(Color::white());
}

WindoW::~WindoW()
{
	std::erase(GUI::windows, this);

	delete header;

	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xSurf);
}

void WindoW::resize(glm::vec2 size)
{
	if ((glm::ivec2)size == (glm::ivec2)this->size) return;
	Debug::funcEntered(__FUNCTION__, size.x, " ", size.y);

	auto prevWSize = this->wSize;
	int w = (int)size.x + (isFullscreen ? 0 : 2 * resizeBorder);
	int h = (int)size.y + (isFullscreen ? 0 : 2 * resizeBorder + headerHeight);
	this->wSize = {w, h};

	Utils::resizeSurface(prevWSize, wSize, wPixelsCapacity, wSurf, wBuf, wPixels, wPixels_pool);
	wl_subsurface_set_position(subsurf, isFullscreen ? 0 : resizeBorder, isFullscreen ? 0 : resizeBorder + headerHeight);

	Component::resize(size);
	Debug::funcExit(__FUNCTION__);
}

void WindoW::switchFullscreen()
{
	if (!GUI::allowSwitchFullscreen) return;
	isFullscreen = !isFullscreen;

	if (window->isFullscreen)
	{
		xdg_toplevel_set_fullscreen(window->top, nullptr);
		header->setActive(false);
	}
	else
	{
		xdg_toplevel_unset_fullscreen(window->top);
		header->setActive(true);
	}
}

void WindoW::frameNew(void* data, wl_callback* cb, uint32_t a)
{
	Debug::funcEntered(__FUNCTION__, "window");
	auto window = (WindoW*)data;

	wl_callback_destroy(cb);
	window->wSurfCallback = wl_surface_frame(window->wSurf);
	wl_callback_add_listener(window->wSurfCallback, &window->wCallbackListener, window);
	wl_surface_commit(window->wSurf);

	Debug::funcExit(__FUNCTION__);
}
void WindoW::configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial)
{
	Debug::funcEntered(__FUNCTION__);
	auto window = (WindoW*)data;
	xdg_surface_ack_configure(xSurf, serial);

	window->resize(window->size + (window->isFullscreen ? glm::vec2(resizeBorder * 2, resizeBorder * 2 + headerHeight) : glm::vec2()));
	//window->draw();
	wl_surface_commit(window->wSurf);
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
	window->resize(glm::vec2(w_, h_) - glm::vec2(2 * resizeBorder, 2 * resizeBorder + headerHeight));
	Debug::funcExit(__FUNCTION__);
}
void WindoW::closeTop(void* data, xdg_toplevel* top) {}
