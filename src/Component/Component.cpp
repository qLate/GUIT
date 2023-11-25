#include "Component.h"

#include <cstring>
#include <string>
#include <unistd.h>
#include <wayland-client-protocol.h>
#include <sys/mman.h>
#include <glm/gtx/string_cast.hpp>

#include "GUIToolkit.h"
#include "Subcomponent.h"
#include "Utils.h"


Component::Component()
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
	wl_surface_destroy(surf);
	wl_buffer_destroy(buf);
}

void Component::update() const
{
	wl_surface_damage_buffer(surf, 0, 0, x, y);
	wl_surface_commit(surf);

	for (auto subComponent : subComponents)
	{
		subComponent->update();
	}
}

void Component::resize(glm::vec2 size)
{
	if (this->size == size) return;

	int len = (int)this->size.x * (int)this->size.y * 4;
	if (len != 0)
	{
		munmap(pixels, len);
		wl_buffer_destroy(buf);
	}

	int len_new = (int)size.x * (int)size.y * 4;
	int32_t fd = Utils::shm_alloc(len_new);
	pixels = (uint8_t*)mmap(nullptr, len_new, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

	wl_shm_pool* pool = wl_shm_create_pool(GUIToolkit::sharedMemory, fd, len_new);
	buf = wl_shm_pool_create_buffer(pool, 0, (int)size.x, (int)size.y, (int)size.x * 4, WL_SHM_FORMAT_ABGR8888);
	wl_surface_attach(surf, buf, 0, 0);
	wl_shm_pool_destroy(pool);
	close(fd);

	if (this->size.x != 0 && this->size.y != 0)
		scaleContent(size);

	for (const auto& subComponent : subComponents)
	{
		subComponent->resizeRec(this->size, size);
	}

	this->size = size;
}
void Component::scaleContent(glm::vec2 size) const
{
	//memset(newPixels, 255, newSize.x * newSize.y * 4);
	//return;

	//auto data = imageData.data();
	auto data = imageData.size() > 10 ? imageData : vector<uint8_t> {0, 0, 255, 255};
	auto factor = imageSize / size;
	for (int y = 0; y < (int)size.y; ++y)
	{
		for (int x = 0; x < (int)size.x; ++x)
		{
			int i = (y * (int)size.x + x) * 4;
			int old_i = ((int)(y * factor.y) * (int)imageSize.x + (int)(x * factor.x)) * 4;
			memcpy(pixels + i, data.data() + old_i, 4);
		}
	}
}

void Component::setColor(Color color)
{
	imageData[0] = color.r * 255;
	imageData[0] = color.g * 255;
	imageData[0] = color.b * 255;
	imageData[0] = color.a * 255;
	imageSize = {1, 1};

	for (int i = 0; i < (int)size.x * (int)size.y * 4; ++i)
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
	resize({w, h});

	imageSize = {w, h};
	memcpy(pixels, imageData.data(), imageData.size());
}

void Component::frameNew(void* data, wl_callback* cb, uint32_t a)
{
	auto component = (Component*)data;

	wl_callback_destroy(cb);
	cb = wl_surface_frame(component->surf);
	wl_callback_add_listener(cb, &component->callbackListener, component);

	component->update();
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
