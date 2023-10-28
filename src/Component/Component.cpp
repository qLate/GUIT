#include "Component.h"

#include <cstring>
#include <string>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <sys/mman.h>

#include "GUIToolkit.h"
#include "Subcomponent.h"
#include "utils.h"
#include "xdg-shell-client-protocol.h"



Component::Component(glm::vec2 size)
{
	surf = wl_compositor_create_surface(GUIToolkit::instance->compositor);
	wl_surface_set_user_data(surf, this);
	wl_surface_add_listener(surf, &surfaceListener, this);

	surfCallback = wl_surface_frame(surf);
	wl_callback_add_listener(surfCallback, &callbackListener, this);

	wl_surface_commit(surf);

	resizeSurface(size);
}
Component::~Component()
{
	destroy();
}

void Component::update() const
{
	wl_surface_attach(surf, buf, 0, 0);
	wl_surface_damage_buffer(surf, 0, 0, x, y);
	wl_surface_commit(surf);
}

void Component::resizeSurface(glm::vec2 size)
{
	if (this->size == size) return;

	if (this->size.x != 0 && this->size.y != 0)
		munmap(pixels, this->size.x * this->size.y * 4);

	int w = size.x;
	int h = size.y;
	int32_t fd = Utils::shm_alloc(w * h * 4);
	pixels = (uint8_t*)(mmap(nullptr, w * h * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0));

	wl_shm_pool* pool = wl_shm_create_pool(GUIToolkit::instance->sharedMemory, fd, w * h * 4);
	buf = wl_shm_pool_create_buffer(pool, 0, w, h, w * 4, WL_SHM_FORMAT_ABGR8888);
	wl_shm_pool_destroy(pool);
	close(fd);

	// Paint
	memset(pixels, rand() % 255, w * h * 4);

	for (const auto& subComponent : subComponents)
	{
		subComponent->resize(this->size, size);
	}

	this->size = size;
}

void Component::destroy()
{
	if (isDestroyed) return;
	isDestroyed = true;

	wl_surface_destroy(surf);
	wl_buffer_destroy(buf);
}

void Component::fillColor(Color color) const
{
	for (int i = 0; i < size.x * size.y * 4; ++i)
	{
		if (i % 4 == 0) pixels[i] = color.b * 255;
		if (i % 4 == 1) pixels[i] = color.g * 255;
		if (i % 4 == 2) pixels[i] = color.r * 255;
		if (i % 4 == 3) pixels[i] = color.a * 255;
	}
}

void Component::frameNew(void* data, wl_callback* cb, uint32_t a)
{
	auto window = (Component*)data;

	wl_callback_destroy(cb);
	//if (window->isClosed) return;
	cb = wl_surface_frame(window->surf);
	wl_callback_add_listener(cb, &window->callbackListener, window);

	window->update();
}
void Component::onSurfaceEnterCallback(void* data, wl_surface* surface, wl_output* output)
{
	auto component = (Component*)data;
	component->onSurfaceEnter(surface, output);
}
void Component::onSurfaceLeaveCallback(void* data, wl_surface* surface, wl_output* output)
{
	auto component = (Component*)data;
	component->onSurfaceLeave(surface, output);
}
