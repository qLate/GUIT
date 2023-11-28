#include "GUIListeners.h"

#include <cstring>
#include <wayland-client-protocol.h>
#include <linux/input-event-codes.h>
#include <wayland-cursor.h>

#include "Debug.h"
#include "GUI.h"
#include "Window.h"
#include "gtx/string_cast.hpp"


void GUIListeners::registerGlobal(void* data, wl_registry* reg, uint32_t name, const char* interface, uint32_t version)
{
	auto toolkit = (GUI*)data;
	if (!strcmp(interface, wl_compositor_interface.name))
	{
		GUI::compositor = (wl_compositor*)wl_registry_bind(reg, name, &wl_compositor_interface, 5);
	}
	else if (!strcmp(interface, wl_shm_interface.name))
	{
		GUI::sharedMemory = (wl_shm*)wl_registry_bind(reg, name, &wl_shm_interface, 1);
	}
	else if (!strcmp(interface, xdg_wm_base_interface.name))
	{
		GUI::sharedBase = (xdg_wm_base*)wl_registry_bind(reg, name, &xdg_wm_base_interface, 1);
		xdg_wm_base_add_listener(GUI::sharedBase, &toolkit->listeners.sharedBaseListener, toolkit);
	}
	else if (!strcmp(interface, wl_seat_interface.name))
	{
		GUI::seat = (wl_seat*)wl_registry_bind(reg, name, &wl_seat_interface, 1);
		wl_seat_add_listener(GUI::seat, &toolkit->listeners.seatListener, toolkit);
	}
	else if (!strcmp(interface, wl_subcompositor_interface.name))
	{
		GUI::subcompositor = (wl_subcompositor*)wl_registry_bind(reg, name, &wl_subcompositor_interface, 1);
	}
}
void GUIListeners::registerGlobalRemove(void* data, wl_registry* reg, uint32_t name) {}

void GUIListeners::seatCap(void* data, wl_seat* seat, uint32_t cap)
{
	auto toolkit = (GUI*)data;
	if (cap & WL_SEAT_CAPABILITY_KEYBOARD && !GUI::keyboard)
	{
		GUI::keyboard = wl_seat_get_keyboard(seat);
		wl_keyboard_add_listener(GUI::keyboard, &toolkit->listeners.keyboardListener, toolkit);
	}
	if (cap & WL_SEAT_CAPABILITY_POINTER)
	{
		GUI::pointer = wl_seat_get_pointer(seat);
		wl_pointer_add_listener(GUI::pointer, &toolkit->listeners.pointerListener, toolkit);
	}
}
void GUIListeners::seatName(void* data, wl_seat* seat, const char* name) {}

void GUIListeners::keyboardKey(void* data, wl_keyboard* kb, uint32_t ser, uint32_t t, uint32_t key, uint32_t state)
{
	if (key == KEY_ESC)
	{
		GUI::instance->closeTrigger = true;
	}
	else if (key == KEY_F)
	{
		if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
			GUI::windows[0]->switchFullscreen();
	}

	if (state == WL_KEYBOARD_KEY_STATE_PRESSED)
		GUI::onKeyDown(key, ser);
	else if (state == WL_KEYBOARD_KEY_STATE_RELEASED)
		GUI::onKeyUp(key, ser);
}
void GUIListeners::pointerButton(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
	Debug::log(LogType::Input, "Pointer button: left: ", std::to_string(button == BTN_LEFT), " down: ", std::to_string(state == WL_POINTER_BUTTON_STATE_PRESSED));
	if (button == BTN_LEFT)
	{
		if (state == WL_POINTER_BUTTON_STATE_PRESSED)
		{
			if (GUI::focusedComponent != nullptr)
				GUI::focusedComponent->onFocusLost();

			GUI::focusedWindow = GUI::hoveredWindow;
			GUI::focusedComponent = GUI::hoveredComponent;
			GUI::focusedSurface = GUI::hoveredSurface;

			GUI::focusedComponent->onFocus();
			GUI::focusedComponent->onPointerDown();
		}
		else if (state == WL_POINTER_BUTTON_STATE_RELEASED)
		{
			GUI::focusedComponent->onPointerUp();
		}
	}

	if (state == WL_POINTER_BUTTON_STATE_PRESSED)
		GUI::onPointerDown(button, serial);
	else if (state == WL_POINTER_BUTTON_STATE_RELEASED)
		GUI::onPointerDown(button, serial);
}

void GUIListeners::pointerMove(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
	GUI::mouseLocalPos = {wl_fixed_to_double(x), wl_fixed_to_double(y)};
	GUI::mousePos = GUI::mouseLocalPos + GUI::hoveredComponent->getTopLeftPos();

	GUI::onPointerMove(GUI::mouseLocalPos);

	Debug::log(LogType::Input, "Pointer move local: ", to_string(GUI::mouseLocalPos));
	Debug::log(LogType::Input, "Pointer move: ", to_string(GUI::mousePos));

	if (GUI::hoveredComponent == nullptr) return;
	updateResize();

	wl_cursor* cursor = wl_cursor_theme_get_cursor(GUI::cursorTheme, Utils::CURSOR_NAMES[GUI::resizeIndex].data());
	wl_cursor_image* cursorImage = cursor->images[0];
	wl_buffer* cursorBuffer = wl_cursor_image_get_buffer(cursorImage);
	wl_surface* cursorSurface = GUI::cursorSurface;

	wl_pointer_set_cursor(pointer, GUI::latestPointerEnterSerial, cursorSurface, cursorImage->hotspot_x, cursorImage->hotspot_y);

	wl_surface_attach(cursorSurface, cursorBuffer, 0, 0);
	wl_surface_damage_buffer(cursorSurface, 0, 0, cursorImage->width, cursorImage->height);
	wl_surface_commit(cursorSurface);

	GUI::cursorImage = cursorImage;
}

void GUIListeners::updateResize()
{
	auto hoveredWindow = GUI::hoveredWindow;
	if (GUI::hoveredSurface != hoveredWindow->wSurf || !hoveredWindow->isResizeable)
	{
		GUI::resizeIndex = 0;
		return;
	}

	auto doResizeX = GUI::mouseLocalPos.x < WindoW::resizeBorder ? 1 : hoveredWindow->wSize.x - GUI::mouseLocalPos.x < WindoW::resizeBorder ? 2 : 0;
	auto doResizeY = GUI::mouseLocalPos.y < WindoW::resizeBorder ? 1 : hoveredWindow->wSize.y - GUI::mouseLocalPos.y < WindoW::resizeBorder ? 2 : 0;
	GUI::resizeIndex = doResizeY + 4 * doResizeX;
}

void GUIListeners::pointerEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y)
{
	GUI::hoveredComponent = (Component*)wl_surface_get_user_data(surface);
	GUI::hoveredWindow = GUI::hoveredComponent->window;
	GUI::hoveredSurface = surface;

	GUI::hoveredComponent->onPointerEnter();

	GUI::latestPointerEnterSerial = serial;
}
void GUIListeners::pointerExit(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
{
	if (GUI::hoveredComponent != nullptr)
		GUI::hoveredComponent->onPointerExit();

	GUI::hoveredWindow = nullptr;
	GUI::hoveredComponent = nullptr;
	GUI::hoveredSurface = nullptr;
}
