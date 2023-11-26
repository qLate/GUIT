#include "Component.h"

#include <string>
#include <wayland-client-protocol.h>
#include <glm/gtx/string_cast.hpp>

#include "GUIToolkit.h"
#include "SubComponent.h"
#include "Utils.h"
#include "Debug.h"


Component::Component(glm::vec2 size)
{
	surf = wl_compositor_create_surface(GUIToolkit::compositor);
	wl_surface_set_user_data(surf, this);
	wl_surface_add_listener(surf, &surfListener, this);

	surfCallback = wl_surface_frame(surf);
	wl_callback_add_listener(surfCallback, &callbackListener, this);
	wl_surface_commit(surf);

	eWindow = wl_egl_window_create(surf, size.x, size.y);
	eSurf = (EGLSurface*)eglCreateWindowSurface(GUIToolkit::eglDisplay, GUIToolkit::eglConfig, eWindow, nullptr);
	cSurf = cairo_gl_surface_create_for_egl(GUIToolkit::cairoDevice, eSurf, size.x, size.y);

	updateImageData();
}
Component::~Component()
{
	wl_callback_destroy(surfCallback);
	wl_surface_destroy(surf);

	wl_egl_window_destroy(eWindow);
	cairo_surface_destroy(cSurf);
	cairo_surface_destroy(imageDataSurf);
}

void Component::update()
{
	Debug::funcEntered(__FUNCTION__);
	auto cr = cairo_create(cSurf);
	cairo_scale(cr, size.x / imageSize.x, size.y / imageSize.y);
	cairo_set_source_surface(cr, imageDataSurf, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
	cairo_paint(cr);

	cairo_gl_surface_swapbuffers(cSurf);
	cairo_destroy(cr);
	Debug::funcExit(__FUNCTION__);
}

void Component::resize(glm::vec2 size)
{
	Debug::funcEntered(__FUNCTION__);
	if (this->size == size)
	{
		Debug::funcExit(__FUNCTION__);
		return;
	}

	wl_egl_window_resize(eWindow, size.x, size.y, 0, 0);
	cairo_gl_surface_set_size(cSurf, size.x, size.y);

	for (const auto& subComponent : subComponents)
	{
		subComponent->resizeRec(this->size, size);
	}

	this->size = size;
	Debug::funcExit(__FUNCTION__);
}

void Component::updateImageData()
{
	imageDataSurf = cairo_image_surface_create_for_data(imageData.data(), CAIRO_FORMAT_ARGB32, imageSize.x, imageSize.y, imageSize.x * 4);
}
void Component::setColor(Color color)
{
	Debug::funcEntered(__FUNCTION__);
	imageSize = {1, 1};
	imageData.resize(4);
	imageData[0] = color.b * 255;
	imageData[1] = color.g * 255;
	imageData[2] = color.r * 255;
	imageData[3] = color.a * 255;

	updateImageData();
	Debug::funcExit(__FUNCTION__);
}
void Component::setImage(const std::string& path)
{
	Debug::funcEntered(__FUNCTION__);
	int w, h;
	Utils::readImage(imageData, path, w, h);
	imageSize = {w, h};

	updateImageData();
	Debug::funcExit(__FUNCTION__);
}

void Component::frameNew(void* data, wl_callback* cb, uint32_t a)
{
	Debug::funcEntered(__FUNCTION__);
	auto component = (Component*)data;

	wl_callback_destroy(cb);
	cb = wl_surface_frame(component->surf);
	wl_callback_add_listener(cb, &component->callbackListener, component);

	component->update();
	Debug::funcExit(__FUNCTION__);
}
void Component::onSurfaceEnterCallback(void* data, wl_surface* surface, wl_output* output)
{
	Debug::funcEntered(__FUNCTION__);
	auto component = (Component*)data;
	component->onSurfaceEnter(surface, output);
	Debug::funcExit(__FUNCTION__);
}
void Component::onSurfaceLeaveCallback(void* data, wl_surface* surface, wl_output* output)
{
	Debug::funcEntered(__FUNCTION__);
	auto component = (Component*)data;
	component->onSurfaceLeave(surface, output);
	Debug::funcExit(__FUNCTION__);
}
