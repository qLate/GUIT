#include "Window.h"

#include <cstring>
#include <unistd.h>
#include <sys/mman.h>

#include "GUIToolkit.h"
#include "utils.h"

Window::Window(const std::string& name, glm::vec2 size) : Component(size)
{
	this->window = this;
	this->doMoveWindow = true;

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

	resizeSurface(size);
}
Window::~Window()
{
	std::erase(GUIToolkit::windows, this);
}
void Window::resizeSurface(glm::vec2 size)
{
	int border = GUIToolkit::windowResizeBorder;
	if (this->size == size) return;

	int len = (this->wSize.x) * (this->wSize.y) * 4;
	if (len != 0)
	{
		munmap(wPixels, len);
		wl_buffer_destroy(wBuf);
	}

	int w = size.x + 2 * border;
	int h = size.y + 2 * border;
	int32_t fd = Utils::shm_alloc(w * h * 4);
	wPixels = (uint8_t*)mmap(nullptr, w * h * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	//memset(wPixels, 0, w * h * 4);

	wl_shm_pool* pool = wl_shm_create_pool(GUIToolkit::sharedMemory, fd, w * h * 4);
	wBuf = wl_shm_pool_create_buffer(pool, 0, w, h, w * 4, WL_SHM_FORMAT_ABGR8888);
	wl_surface_attach(wSurf, wBuf, 0, 0);
	wl_shm_pool_destroy(pool);
	close(fd);

	wl_subsurface_set_position(subsurf, border, border);

	this->wSize = {w, h};

	Component::resizeSurface(size);
}
void Window::update() const
{
	Component::update();

	wl_surface_damage_buffer(wSurf, 0, 0, size.x + 2 * GUIToolkit::windowResizeBorder, size.y + 2 * GUIToolkit::windowResizeBorder);
	wl_surface_commit(wSurf);
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
		window->resizeSurface(glm::vec2(w_, h_) - glm::vec2(2 * GUIToolkit::windowResizeBorder, 2 * GUIToolkit::windowResizeBorder));
}
void Window::closeTop(void* data, xdg_toplevel* top)
{
	auto window = (Window*)data;
	window->destroy();
}
