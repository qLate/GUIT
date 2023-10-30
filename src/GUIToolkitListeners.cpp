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

void GUIToolkitListeners::keyboardKey(void* data, wl_keyboard* kb, uint32_t ser, uint32_t t, uint32_t key, uint32_t stat)
{
	auto toolkit = (GUIToolkit*)data;
	if (key == 1) toolkit->closeTrigger = true;
	else if (key == 30)
	{
		std::cout << "a\n";
	}
	else if (key == 32)
	{
		std::cout << "d\n";
	}
}
void GUIToolkitListeners::pointerButton(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state)
{
	if (button == BTN_LEFT && state == WL_POINTER_BUTTON_STATE_PRESSED)
	{
		auto component = GUIToolkit::focusedComponent;
		if (component == nullptr)return;

		auto window = component->window;
		float resizeBorder = GUIToolkit::windowResizeBorder;
		auto pointerPos = GUIToolkit::pointerPos;
		int doResizeX = pointerPos.x < resizeBorder ? 1 : window->size.x - pointerPos.x < resizeBorder ? 2 : 0;
		int doResizeY = pointerPos.y < resizeBorder ? 1 : window->size.y - pointerPos.y < resizeBorder ? 2 : 0;
		if (window->isResizeable && (doResizeX || doResizeY))
		{
			int idx = doResizeY + 4 * doResizeX;
			xdg_toplevel_resize(window->top, GUIToolkit::seat, serial, idx);
		}
		else if (component->doMoveWindow && component->window->isMoveable)
		{
			xdg_toplevel_move(window->top, GUIToolkit::seat, serial);
		}
	}
}

const std::vector<std::string> CURSOR_NAMES = {
	"left_ptr", "top_side", "bottom_side", "", "left_side", "top_left_corner",
	"bottom_left_corner", "", "right_side", "top_right_corner", "bottom_right_corner"
};
int prevDoResizeX = 0;
int prevDoResizeY = 0;
void GUIToolkitListeners::pointerMove(void* data, wl_pointer* pointer, uint32_t time, wl_fixed_t x, wl_fixed_t y)
{
	glm::vec2 newPointerPos = {wl_fixed_to_double(x), wl_fixed_to_double(y)};
	GUIToolkit::pointerPos = newPointerPos;
	GUIToolkit::onPointerMove(newPointerPos);

	auto component = GUIToolkit::focusedComponent;
	if (component == nullptr)return;

	auto window = component->window;
	float resizeBorder = GUIToolkit::windowResizeBorder;
	auto pointerPos = GUIToolkit::pointerPos;
	int doResizeX = pointerPos.x < resizeBorder ? 1 : window->wSize.x - pointerPos.x < resizeBorder ? 2 : 0;
	int doResizeY = pointerPos.y < resizeBorder ? 1 : window->wSize.y - pointerPos.y < resizeBorder ? 2 : 0;
	if (prevDoResizeX != doResizeX || prevDoResizeY != doResizeY)
	{
		int idx;
		if (GUIToolkit::focusedSurface == GUIToolkit::focusedComponent->window->wSurf && (window->isResizeable && (doResizeX || doResizeY)))
		{
			idx = doResizeY + 4 * doResizeX;
			prevDoResizeX = doResizeX;
			prevDoResizeY = doResizeY;

			std::cout << CURSOR_NAMES[idx] << '\n';
		}
		else
		{
			idx = 0;
			prevDoResizeX = 0;
			prevDoResizeY = 0;
		}

		wl_cursor* cursor = wl_cursor_theme_get_cursor(GUIToolkit::cursorTheme, CURSOR_NAMES[idx].data());
		wl_cursor_image* cursorImage = cursor->images[0];
		wl_buffer* cursorBuffer = wl_cursor_image_get_buffer(cursorImage);

		wl_surface_attach(GUIToolkit::cursorSurface, cursorBuffer, 0, 0);
		wl_surface_damage_buffer(GUIToolkit::cursorSurface, 0, 0, cursorImage->width, cursorImage->height);
		wl_surface_commit(GUIToolkit::cursorSurface);

		wl_pointer_set_cursor(pointer, GUIToolkit::latestPointerEnterSerial, GUIToolkit::cursorSurface, cursorImage->hotspot_x, cursorImage->hotspot_y);
		GUIToolkit::cursorImage = cursorImage;
	}
}
void GUIToolkitListeners::pointerEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t y)
{
	GUIToolkit::focusedComponent = (Component*)wl_surface_get_user_data(surface);
	GUIToolkit::focusedSurface = surface;
	GUIToolkit::latestPointerEnterSerial = serial;

	prevDoResizeX = -1;
	prevDoResizeY = -1;
}
void GUIToolkitListeners::pointerExit(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface)
{
	GUIToolkit::focusedComponent = nullptr;
}
