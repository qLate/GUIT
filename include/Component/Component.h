#pragma once

#include "wayland-client.h"
#include "Color.h"
#include "vec2.hpp"

class Window;

class Component
{
protected:
	wl_callback_listener callbackListener {
		.done = frameNew
	};
	wl_surface_listener surfaceListener = {
		.enter = onSurfaceEnterCallback,
		.leave = onSurfaceLeaveCallback,
	};

	bool isDestroyed = false;

public:
	wl_surface* surf = nullptr;
	wl_callback* surfCallback = nullptr;
	wl_buffer* buf = nullptr;

	glm::vec2 size;
	uint8_t* pixels = nullptr;
	int x = 0, y = 0;

	Window* window;
	bool isWindowMoveable;


	Component(glm::vec2 size);
	virtual ~Component();

	void update() const;
	void resize(glm::vec2 size);

	void fillColor(Color color) const;

	virtual void destroy();

	virtual void onSurfaceEnter(wl_surface* surface, wl_output* output) {}
	virtual void onSurfaceLeave(wl_surface* surface, wl_output* output) {}

	static void frameNew(void* data, wl_callback* cb, uint32_t a);
	static void onSurfaceEnterCallback(void* data, wl_surface* surface, wl_output* output);
	static void onSurfaceLeaveCallback(void* data, wl_surface* surface, wl_output* output);
	friend class GUIToolkitListeners;
};
