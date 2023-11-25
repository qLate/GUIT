#pragma once

#include <string>
#include <vector>

#include "wayland-client.h"
#include "Color.h"
#include "vec2.hpp"

class SubComponent;
class Window;

class Component
{
protected:
	wl_callback_listener callbackListener {
		.done = frameNew
	};
	wl_surface_listener surfListener = {
		.enter = onSurfaceEnterCallback,
		.leave = onSurfaceLeaveCallback,
	};

	uint8_t* pixels = nullptr;
	int pixelsCapacity = 0;
	wl_shm_pool* pixels_pool = nullptr;

	glm::vec2 imageSize {1, 1};
	std::vector<uint8_t> imageData {255, 255, 255, 255};

public:
	std::vector<SubComponent*> subComponents {};

	wl_surface* surf = nullptr;
	wl_callback* surfCallback = nullptr;
	wl_buffer* buf = nullptr;

	glm::vec2 size;
	int x = 0, y = 0;

	Window* window;
	bool moveWindowOnDrag;

	Component();
	virtual ~Component();

	virtual void update() const;

	virtual void resize(glm::vec2 size);
	void scaleContent(glm::vec2 size) const;

	void setColor(Color color);
	void setImage(const std::string& path);

	virtual void onSurfaceEnter(wl_surface* surface, wl_output* output) {}
	virtual void onSurfaceLeave(wl_surface* surface, wl_output* output) {}

	static void frameNew(void* data, wl_callback* cb, uint32_t a);
	static void onSurfaceEnterCallback(void* data, wl_surface* surface, wl_output* output);
	static void onSurfaceLeaveCallback(void* data, wl_surface* surface, wl_output* output);
	friend class GUIToolkitListeners;
};
