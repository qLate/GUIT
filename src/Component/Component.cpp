#include "Component.h"

#include <string>
#include <wayland-client-protocol.h>
#include <glm/gtx/string_cast.hpp>

#include "GUI.h"
#include "SubComponent.h"
#include "Utils.h"
#include "Debug.h"
#include "Window.h"

Component::Component(glm::vec2 size)
{
	surf = wl_compositor_create_surface(GUI::compositor);
	wl_surface_set_user_data(surf, this);

	surfCallback = wl_surface_frame(surf);
	wl_callback_add_listener(surfCallback, &callbackListener, this);
	wl_surface_commit(surf);

	eWindow = wl_egl_window_create(surf, size.x, size.y);
	//eSurf = (EGLSurface*)eglCreateWindowSurface(GUI::eglDisplay, GUI::eglConfig, eWindow, nullptr);
	//cSurf = cairo_gl_surface_create_for_egl(GUI::cairoDevice, eSurf, size.x, size.y);
}
Component::~Component()
{
	wl_callback_destroy(surfCallback);
	wl_surface_destroy(surf);

	cairo_surface_destroy(rSurf);
	eglDestroySurface(GUI::eglDisplay, eSurf);
	cairo_surface_destroy(cSurf);
	wl_egl_window_destroy(eWindow);
}

void Component::draw()
{
	if (!isActive) return;
	Debug::funcEntered(__FUNCTION__);

	if (needSurfaceResize)
		forceUpdateSurfaces();

	auto cr = cairo_create(cSurf);

	if (scaleImage)
		cairo_scale(cr, size.x / imageSize.x, size.y / imageSize.y);
	cairo_set_source_surface(cr, rSurf, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
	cairo_paint(cr);

	cairo_gl_surface_swapbuffers(cSurf);
	cairo_destroy(cr);
	wl_surface_commit(surf);
	Debug::funcExit(__FUNCTION__);
}
void Component::forceUpdateSurfaces()
{
	//needSurfaceResize = false;

	eglDestroySurface(GUI::eglDisplay, eSurf);
	cairo_surface_destroy(cSurf);

	wl_egl_window_resize(eWindow, size.x, size.y, 0, 0);

	eSurf = (EGLSurface*)eglCreateWindowSurface(GUI::eglDisplay, GUI::eglConfig, eWindow, nullptr);
	cSurf = cairo_gl_surface_create_for_egl(GUI::cairoDevice, eSurf, size.x, size.y);
}

void Component::resize(glm::vec2 size)
{
	if (!isActive) return;
	auto finalSize = preserveAspect ? Utils::getPreservedAspect(imageSize, size) : size;
	if (finalSize == this->size) return;

	auto prevSize = this->size;
	this->size = finalSize;
	needSurfaceResize = true;

	for (const auto& subComponent : subComponents)
	{
		subComponent->resizeRec(prevSize, this->size);
	}
}

void Component::setColor(Color color)
{
	imageSize = size;

	rSurf = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
	auto cr = cairo_create(rSurf);

	cairo_set_source_rgb(cr, color.r, color.g, color.b);
	cairo_paint(cr);

	cairo_destroy(cr);
}
void Component::setImage(const std::string& path)
{
	int w, h;
	imageData.clear();
	Utils::readImage(imageData, path, w, h);
	imageSize = {w, h};

	if (this != window)
		this->preserveAspect = true;
	this->scaleImage = true;
	resize(size);

	rSurf = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
	auto imageSurf = cairo_image_surface_create_for_data(imageData.data(), CAIRO_FORMAT_ARGB32, w, h, w * 4);
	auto cr = cairo_create(rSurf);

	//cairo_scale(cr, size.x / w, size.y / h);
	cairo_set_source_surface(cr, imageSurf, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
	cairo_paint(cr);

	cairo_destroy(cr);
}

void Component::frameNew(void* data, wl_callback* cb, uint32_t a)
{
	auto component = (Component*)data;
	Debug::funcEntered(__FUNCTION__, "component ", component->nameID);

	wl_callback_destroy(cb);
	component->surfCallback = wl_surface_frame(component->surf);
	wl_callback_add_listener(component->surfCallback, &component->callbackListener, component);

	component->draw();
	Debug::funcExit(__FUNCTION__);
}

void Component::onPointerEnter()
{
	OnPointerEnter();
}
void Component::onPointerExit()
{
	OnPointerExit();
}
void Component::onPointerDown()
{
	OnPointerDown();
}
void Component::onPointerUp()
{
	OnPointerUp();
}
void Component::onFocus()
{
	OnFocus();
}
void Component::onFocusLost()
{
	OnFocusLost();
}
