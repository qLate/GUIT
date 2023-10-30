#include "Component.h"

#include <cstring>
#include <string>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <sys/mman.h>

#include "GUIToolkit.h"
#include "Subcomponent.h"
#include "utils.h"


Component::Component(glm::vec2 size)
{
	surf = wl_compositor_create_surface(GUIToolkit::compositor);
	wl_surface_set_user_data(surf, this);
	wl_surface_add_listener(surf, &surfListener, this);

	surfCallback = wl_surface_frame(surf);
	wl_callback_add_listener(surfCallback, &callbackListener, this);
	wl_surface_commit(surf);
}
Component::~Component()
{
	destroy();
}

void Component::update() const
{
	wl_surface_damage_buffer(surf, 0, 0, x, y);
	wl_surface_commit(surf);
}

void Component::resizeSurface(glm::vec2 size)
{
	if (this->size == size) return;

	int len = this->size.x * this->size.y * 4;
	if (len != 0)
	{
		munmap(pixels, len);
		wl_buffer_destroy(buf);
	}

	int w = size.x;
	int h = size.y;
	int32_t fd = Utils::shm_alloc(w * h * 4);
	pixels = (uint8_t*)mmap(nullptr, w * h * 4, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	wl_shm_pool* pool = wl_shm_create_pool(GUIToolkit::sharedMemory, fd, w * h * 4);
	buf = wl_shm_pool_create_buffer(pool, 0, w, h, w * 4, WL_SHM_FORMAT_ABGR8888);
	wl_surface_attach(surf, buf, 0, 0);
	wl_shm_pool_destroy(pool);
	close(fd);

	if (this->size.x != 0 && this->size.y != 0)
		scaleContent(imageData.data(), imageSize, pixels, size);

	for (const auto& subComponent : subComponents)
	{
		subComponent->resize(this->size, size);
	}

	this->size = size;
}
void Component::scaleContent(const uint8_t* oldPixels, glm::vec2 oldSize, uint8_t* newPixels, glm::vec2 newSize)
{
	auto factor = oldSize / newSize;
	for (int y = 0; y < (int)newSize.y; ++y)
	{
		for (int x = 0; x < (int)newSize.x; ++x)
		{
			int i = (y * newSize.x + x) * 4;
			int old_i = (((int)(y * factor.y) * oldSize.x) + (int)(x * factor.x)) * 4;
			memcpy(newPixels + i, oldPixels + old_i, 4);
		}
	}
}

void Component::destroy()
{
	if (isDestroyed) return;
	isDestroyed = true;

	wl_surface_destroy(surf);
	wl_buffer_destroy(buf);
}

void Component::setColor(Color color) const
{
	for (int i = 0; i < size.x * size.y * 4; ++i)
	{
		if (i % 4 == 0) pixels[i] = color.r * 255;
		if (i % 4 == 1) pixels[i] = color.g * 255;
		if (i % 4 == 2) pixels[i] = color.b * 255;
		if (i % 4 == 3) pixels[i] = color.a * 255;
	}
}
void Component::setImage(const std::string& path)
{
	int w, h;
	Utils::readImage(imageData, path, w, h);
	resizeSurface({w, h});

	imageSize = {w, h};
	memcpy(pixels, imageData.data(), imageData.size());
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
