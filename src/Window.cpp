#include "Window.h"

#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <sys/mman.h>

#include "GUIToolkit.h"
#include "utils.h"
#include "xdg-shell-client-protocol.h"

void Window::draw() const
{
	wl_surface_attach(surf, buf, 0, 0);
	wl_surface_damage_buffer(surf, 0, 0, w, h);
	wl_surface_commit(surf);
}

Window::Window(const std::string& name, int w, int h)
{
	GUIToolkit::windows.push_back(this);

	surf = wl_compositor_create_surface(GUIToolkit::instance->compositor);
	wl_surface_set_user_data(surf, this);
	wl_surface_add_listener(surf, &listeners.surface_listener, this);

	surfCallback = wl_surface_frame(surf);
	wl_callback_add_listener(surfCallback, &listeners.callbackListener, this);

	xSurf = xdg_wm_base_get_xdg_surface(GUIToolkit::instance->sharedBase, surf);
	xdg_surface_add_listener(xSurf, &listeners.xSurfListener, this);

	top = xdg_surface_get_toplevel(xSurf);
	xdg_toplevel_add_listener(top, &listeners.topListener, this);
	xdg_toplevel_set_title(top, name.data());

	wl_surface_commit(surf);

	resize(w, h);
}
Window::~Window()
{
	std::erase(GUIToolkit::windows, this);

	wClose();
}

void Window::wClose()
{
	if (isClosed) return;
	isClosed = true;

	wl_surface_destroy(surf);
	wl_buffer_destroy(buf);
	xdg_toplevel_destroy(top);
	xdg_surface_destroy(xSurf);
}

void Window::resize(int w_, int h_)
{
	if (w_ == w && h_ == h) return;
	if (w != 0 && h != 0)
		munmap(pixels, w * h * 4);

	this->w = w_;
	this->h = h_;

	int32_t fd = Utils::shm_alloc(w * h * 4);
	pixels = (uint8_t*)(mmap(0, w * h * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

	wl_shm_pool* pool = wl_shm_create_pool(GUIToolkit::instance->sharedMemory, fd, w * h * 4);
	buf = wl_shm_pool_create_buffer(pool, 0, w, h, w * 4, WL_SHM_FORMAT_ARGB8888);
	wl_shm_pool_destroy(pool);
	close(fd);

	// Paint
	memset(pixels, 255, w * h * 4);

	for (int i = 0; i < w * h * 4; ++i)
	{
		if (i % 4 != 3)
			pixels[i] = rand();
	}
}
void Window::setPos(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Window::fillColor(Color color) const
{
	for (int i = 0; i < w * h * 4; ++i)
	{
		if (i % 4 == 0) pixels[i] = color.b * 255;
		if (i % 4 == 1) pixels[i] = color.g * 255;
		if (i % 4 == 2) pixels[i] = color.r * 255;
		if (i % 4 == 3) pixels[i] = color.a * 255;
	}
}
