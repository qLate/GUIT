#pragma once

#include <string>
#include <vector>

#include "wayland-client.h"
#include "Color.h"
#include "vec2.hpp"
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>

class SubComponent;
class WindoW;

class Component
{
protected:
	wl_callback_listener callbackListener {
		.done = frameNew
	};

	glm::ivec2 imageSize {1, 1};
	std::vector<uint8_t> imageData {255, 255, 255, 255};
	std::vector<uint8_t> scaledImageData {255, 255, 255, 255};
	cairo_surface_t* imageDataSurf;

public:
	std::vector<SubComponent*> subComponents {};

	wl_surface* surf = nullptr;
	wl_callback* surfCallback = nullptr;

	wl_egl_window* eWindow = nullptr;
	EGLSurface eSurf = nullptr;
	cairo_surface_t* cSurf = nullptr;

	glm::vec2 pos = {0, 0};
	glm::vec2 size;

	WindoW* window;


	Component(glm::vec2 size);
	virtual ~Component();

	virtual void draw();
	virtual void resize(glm::vec2 size);

	void updateImageData();
	void setColor(Color color);
	void setImage(const std::string& path);

	virtual glm::vec2 getTopLeftPos() const { return pos; };

private:
	virtual void onPointerEnter() {}
	virtual void onPointerExit() {}
	virtual void onPointerDown() {}
	virtual void onPointerUp() {}

	static void frameNew(void* data, wl_callback* cb, uint32_t a);

	friend class GUIListeners;
	friend class Utils;
};
