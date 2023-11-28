#pragma once

#include <string>
#include <vector>

#include "wayland-client.h"
#include "Color.h"
#include "vec2.hpp"
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>

#include "Action.h"

class SubComponent;
class WindoW;

class Component
{
protected:
	wl_callback_listener callbackListener {
		.done = frameNew
	};

	bool scaleImage = false;
	std::vector<uint8_t> imageData {};
	glm::vec2 imageSize = {1, 1};

public:
	std::string nameID;

	WindoW* window;
	std::vector<SubComponent*> subComponents {};

	wl_surface* surf = nullptr;
	wl_callback* surfCallback = nullptr;

	wl_egl_window* eWindow = nullptr;
	EGLSurface eSurf = nullptr;
	cairo_surface_t* cSurf = nullptr;
	cairo_surface_t* rSurf = nullptr;
	bool needSurfaceResize = true;

	glm::vec2 pos = {0, 0};
	glm::vec2 size;
	bool preserveAspect = false;
	bool isActive = true;

	Action<> OnPointerEnter {};
	Action<> OnPointerExit {};
	Action<> OnPointerDown {};
	Action<> OnPointerUp {};
	Action<> OnFocus {};
	Action<> OnFocusLost {};


	Component(glm::vec2 size);
	virtual ~Component();

	void draw();
	void forceUpdateSurfaces();

	virtual void resize(glm::vec2 size);

	virtual void setColor(Color color);
	void setImage(const std::string& path);

	virtual glm::vec2 getTopLeftPos() const { return pos; };

private:
	virtual void onPointerEnter();
	virtual void onPointerExit();
	virtual void onPointerDown();
	virtual void onPointerUp();
	virtual void onFocus();;
	virtual void onFocusLost();;

	static void frameNew(void* data, wl_callback* cb, uint32_t a);

	friend class GUIListeners;
	friend class Utils;
};
