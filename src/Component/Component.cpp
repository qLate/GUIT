#include "Component.h"

#include <string>
#include <wayland-client-protocol.h>
#include <glm/gtx/string_cast.hpp>

#include "GUI.h"
#include "SubComponent.h"
#include "Utils.h"
#include "Debug.h"

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

	updateRecordSurface();
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
		subComponent->resizeRec(prevSize, size);
	}
}

void Component::setColor(Color color)
{
	imageSize = {1, 1};
	imageData.resize(4);
	imageData[0] = color.b * 255;
	imageData[1] = color.g * 255;
	imageData[2] = color.r * 255;
	imageData[3] = color.a * 255;

	updateRecordSurface();
}
void Component::setImage(const std::string& path)
{
	int w, h;
	Utils::readImage(imageData, path, w, h);
	imageSize = {w, h};

	preserveAspect = true;
	resize(size);

	updateRecordSurface();
}
void Component::updateRecordSurface()
{
	if (rSurf != nullptr)
		cairo_surface_destroy(rSurf);

	auto imageSurf = cairo_image_surface_create_for_data(imageData.data(), CAIRO_FORMAT_ARGB32, imageSize.x, imageSize.y, imageSize.x * 4);
	rSurf = cairo_recording_surface_create(CAIRO_CONTENT_COLOR_ALPHA, nullptr);
	auto cr = cairo_create(rSurf);

	cairo_set_source_surface(cr, imageSurf, 0, 0);
	cairo_pattern_set_filter(cairo_get_source(cr), CAIRO_FILTER_NEAREST);
	cairo_paint(cr);

	// Draw transparent squares
	//for (int i = 1; i <= 10; i++)
	//{
	//	cairo_set_source_rgba(cr, 0, 1, 1, i * 0.1);
	//	cairo_rectangle(cr, 50 * i, 20, 40, 40);
	//	cairo_fill(cr);
	//}

	cairo_destroy(cr);
}

void Component::setActive(bool isActive)
{
	if (this->isActive == isActive) return;
	this->isActive = isActive;

	if (!isActive)
	{
		eglDestroySurface(GUI::eglDisplay, eSurf);
		cairo_surface_destroy(cSurf);

		wl_egl_window_resize(eWindow, 1, 1, 0, 0);

		eSurf = (EGLSurface*)eglCreateWindowSurface(GUI::eglDisplay, GUI::eglConfig, eWindow, nullptr);
		cSurf = cairo_gl_surface_create_for_egl(GUI::cairoDevice, eSurf, size.x, size.y);

		//auto cr = cairo_create(cSurf);
		//cairo_set_source_rgba(cr, 1, 1, 1, 1);
		//cairo_paint(cr);

		cairo_gl_surface_swapbuffers(cSurf);
		//cairo_destroy(cr);
	}
	else
	{
		wl_egl_window_resize(eWindow, size.x, size.y, 0, 0);
		cairo_gl_surface_set_size(cSurf, size.x, size.y);
	}

	for (const auto& subComponent : subComponents)
		subComponent->setActive(isActive);
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
