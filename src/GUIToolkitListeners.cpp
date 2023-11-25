#include "GUIToolkitListeners.h"

#include <cstring>
#include <iostream>
#include <wayland-client-protocol.h>
#include <linux/input-event-codes.h>
#include <wayland-cursor.h>

#include "GUIToolkit.h"
#include "Window.h"


void GUIToolkitListeners::registerGlobal(void* data, wl_registry* reg, uint32_t name, const char* interface, uint32_t version)
{
	auto toolkit = (GUIToolkit*)data;

	if (!strcmp(interface, wl_compositor_interface.name))
	{
		GUIToolkit::compositor = (wl_compositor*)wl_registry_bind(reg, name, &wl_compositor_interface, 5);
	}
	else if (!strcmp(interface, wl_shm_interface.name))
	{
		GUIToolkit::sharedMemory = (wl_shm*)wl_registry_bind(reg, name, &wl_shm_interface, 1);
	}
	else if (!strcmp(interface, xdg_wm_base_interface.name))
	{
		GUIToolkit::sharedBase = (xdg_wm_base*)wl_registry_bind(reg, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(GUIToolkit::sharedBase, &toolkit->listeners.sharedBaseListener, toolkit);
	}
	else if (!strcmp(interface, wl_seat_interface.name))
	{
		GUIToolkit::seat = (wl_seat*)wl_registry_bind(reg, name, &wl_seat_interface, 1);
		wl_seat_add_listener(GUIToolkit::seat, &toolkit->listeners.seatListener, toolkit);
	}
	else if (!strcmp(interface, wl_subcompositor_interface.name))
	{
		GUIToolkit::subcompositor = (wl_subcompositor*)wl_registry_bind(reg, name, &wl_subcompositor_interface, 1);
	}
}
void GUIToolkitListeners::registerGlobalRemove(void* data, wl_registry* reg, uint32_t name) {}

void GUIToolkitListeners::seatCap(void* data, wl_seat* seat, uint32_t cap)
{
	auto toolkit = (GUIToolkit*)data;
	if (cap & WL_SEAT_CAPABILITY_KEYBOARD && !GUIToolkit::keyboard)
	{
		GUIToolkit::keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(GUIToolkit::keyboard, &toolkit->listeners.keyboardListener, toolkit);
	}
	if (cap & WL_SEAT_CAPABILITY_POINTER)
	{
		GUIToolkit::pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(GUIToolkit::pointer, &toolkit->listeners.pointerListener, toolkit);
	}
}
void GUIToolkitListeners::seatName(void* data, wl_seat* seat, const char* name) {}

void GUIToolkitListeners::keyboardKey(void* data, wl_keyboard* kb, uint32_t ser, uint32_t t, uint32_t key, uint32_t state)
{
	auto toolkit = (GUIToolkit*)data;
	if (key == 1) toolkit->closeTrigger = true;
	else if (key == KEY_F)
	{
		if (state == WL_KEYBOARD_KEY_STATE_PRESSED && GUIToolkit::focusedWindow != nullptr)
			GUIToolkit::focusedWindow->switchFullscreen();
	}
}
void GUIToolkitListeners::pointerButton(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
	if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
	{
		// Set focused window
		GUIToolkit::focusedWindow = GUIToolkit::hoveredWindow;
		GUIToolkit::focusedComponent = GUIToolkit::hoveredComponent;

		// Resize or move window
		auto window = GUIToolkit::focusedWindow;
		if (window->isResizeable && GUIToolkit::resizeIndex != 0)
		{
			xdg_toplevel_resize(window->top, GUIToolkit::seat, serial, GUIToolkit::resizeIndex);
		}
		else if (window->isMoveable && GUIToolkit::focusedComponent->moveWindowOnDrag)
		{
			xdg_toplevel_move(window->top, GUIToolkit::seat, serial);
		}
	}
}

void GUIToolkitListeners::pointerMove(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
	GUIToolkit::mousePos = {wl_fixed_to_double(x), wl_fixed_to_double(y)};
	GUIToolkit::onPointerMove(GUIToolkit::mousePos);

	if (GUIToolkit::hoveredComponent == nullptr) return;

	updateResize();

	wl_cursor* cursor = wl_cursor_theme_get_cursor(GUIToolkit::cursorTheme, Utils::CURSOR_NAMES[GUIToolkit::resizeIndex].data());
	wl_cursor_image* cursorImage = cursor->images[0];
	wl_buffer* cursorBuffer = wl_cursor_image_get_buffer(cursorImage);
	wl_surface* cursorSurface = GUIToolkit::cursorSurface;

	wl_pointer_set_cursor(pointer, GUIToolkit::latestPointerEnterSerial, cursorSurface, cursorImage->hotspot_x, cursorImage->hotspot_y);

	wl_surface_attach(cursorSurface, cursorBuffer, 0, 0);
	wl_surface_damage_buffer(cursorSurface, 0, 0, cursorImage->width, cursorImage->height);
	wl_surface_commit(cursorSurface);

	GUIToolkit::cursorImage = cursorImage;
}

void GUIToolkitListeners::updateResize()
{
	auto hoveredWindow = GUIToolkit::hoveredWindow;
	if (GUIToolkit::hoveredSurface != hoveredWindow->wSurf || !hoveredWindow->isResizeable)
	{
		GUIToolkit::resizeIndex = 0;
		return;
	}

	auto doResizeX = GUIToolkit::mousePos.x < GUIToolkit::windowResizeBorder ? 1 : hoveredWindow->wSize.x - GUIToolkit::mousePos.x < GUIToolkit::windowResizeBorder ? 2 : 0;
	auto doResizeY = GUIToolkit::mousePos.y < GUIToolkit::windowResizeBorder ? 1 : hoveredWindow->wSize.y - GUIToolkit::mousePos.y < GUIToolkit::windowResizeBorder ? 2 : 0;
	GUIToolkit::resizeIndex = doResizeY + 4 * doResizeX;
}

void GUIToolkitListeners::pointerEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y)
{
	auto focusedComponent = (Component*)wl_surface_get_user_data(surface);
	GUIToolkit::hoveredWindow = focusedComponent->window;
	GUIToolkit::hoveredComponent = focusedComponent;
	GUIToolkit::hoveredSurface = surface;

	GUIToolkit::latestPointerEnterSerial = serial;
}
void GUIToolkitListeners::pointerExit(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
{
	GUIToolkit::focusedWindow = nullptr;
	GUIToolkit::focusedComponent = nullptr;
	GUIToolkit::hoveredSurface = nullptr;
}
