#include "Window.h"

#include <cstring>
#include <unistd.h>
#include <sys/mman.h>

#include "GUIToolkit.h"
#include "Utils.h"

Window::Window(const std::string& name, glm::vec2 size)
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

	GUIToolkit::windows.push_back(this);

	resize(size);
}
Window::~Window()
{
	std::erase(GUIToolkit::windows, this);

	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xSurf);
}
void Window::resize(glm::vec2 size)
{
	int border = GUIToolkit::windowResizeBorder;
	if (this->size == size) return;

	int len = (this->wSize.x) * (this->wSize.y) * 4;
	if (len != 0)
		wl_buffer_destroy(wBuf);

	int w = (int)size.x + (isFullscreen ? 0 : 2 * border);
	int h = (int)size.y + (isFullscreen ? 0 : 2 * border);
	int len_new = w * h * 4;
	if (len_new > wPixelsCapacity || len_new < wPixelsCapacity / 3)
	{
		if (wPixels_pool != nullptr)
		{
			wl_shm_pool_destroy(wPixels_pool);
			munmap(wPixels, wPixelsCapacity);
		}

		wPixelsCapacity = len_new * 2;
		auto fd = Utils::shm_alloc(wPixelsCapacity);
		wPixels = (uint8_t*)mmap(nullptr, wPixelsCapacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		wPixels_pool = wl_shm_create_pool(GUIToolkit::sharedMemory, fd, wPixelsCapacity);
	}

	wBuf = wl_shm_pool_create_buffer(wPixels_pool, 0, w, h, w * 4, WL_SHM_FORMAT_ABGR8888);
	wl_surface_attach(wSurf, wBuf, 0, 0);

	wl_subsurface_set_position(subsurf, isFullscreen ? 0 : border, isFullscreen ? 0 : border);

	this->wSize = {w, h};

	Component::resize(size);
}
void Window::update() const
{
	Component::update();

	wl_surface_damage_buffer(wSurf, 0, 0, size.x + 2 * GUIToolkit::windowResizeBorder, size.y + 2 * GUIToolkit::windowResizeBorder);
	wl_surface_commit(wSurf);
}
void Window::switchFullscreen()
{
	isFullscreen = !isFullscreen;

	if (window->isFullscreen)
		xdg_toplevel_set_fullscreen(window->top, nullptr);
	else
		xdg_toplevel_unset_fullscreen(window->top);
}

void Window::configureXSurf(void* data, xdg_surface* xSurf, uint32_t serial)
{
	auto window = (Window*)data;
	xdg_surface_ack_configure(xSurf, serial);

	window->resize(window->size + (window->isFullscreen ? glm::vec2(GUIToolkit::windowResizeBorder * 2, GUIToolkit::windowResizeBorder * 2) : glm::vec2()));
	window->update();
}
void Window::configureTop(void* data, xdg_toplevel* xSurf, int32_t w_, int32_t h_, wl_array* stat)
{
	if (!w_ && !h_) return;

	auto window = (Window*)data;
	if (window->size.x != w_ || window->size.y != h_)
		window->resize(glm::vec2(w_, h_) - glm::vec2(2 * GUIToolkit::windowResizeBorder, 2 * GUIToolkit::windowResizeBorder));
}
void Window::closeTop(void* data, xdg_toplevel* top) {}
