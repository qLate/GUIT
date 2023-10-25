#pragma once

#include <wayland-client-protocol.h>
#include "xdg-shell-client-protocol.h"

class GUIToolkit;

class GUIToolkitListeners
{
	wl_registry_listener registryListener = {
		.global = registerGlobal,
		.global_remove = registerGlobalRemove
	};

	wl_keyboard_listener keyboardListener = {
		.keymap = [](void* data, wl_keyboard* kb, uint32_t surf, int32_t fd, uint32_t sz) {},
		.enter = [](void* data, wl_keyboard* kb, uint32_t ser, wl_surface* surf, wl_array* keys) {},
		.leave = [](void* data, wl_keyboard* kb, uint32_t ser, wl_surface* surf) {},
		.key = keyboardKey,
		.modifiers = [](void* data, wl_keyboard* kb, uint32_t ser, uint32_t dep, uint32_t lat, uint32_t lock, uint32_t grp) {},
		.repeat_info = [](void* data, wl_keyboard* kb, int32_t rate, int32_t del) {}
	};

	wl_pointer_listener pointerListener = {
		.enter = pointerEnter,
		.leave = pointerExit,
		.motion = [](void*, struct wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t) {},
		.button = pointerButton,
		.axis = [](void*, struct wl_pointer*, uint32_t, uint32_t, wl_fixed_t) {},
	};

	wl_seat_listener seatListener = {
		.capabilities = seatCap,
		.name = seatName
	};

	xdg_wm_base_listener sharedBaseListener = {
		.ping = [](void*, xdg_wm_base* sh, uint32_t ser) { xdg_wm_base_pong(sh, ser); }
	};


	static void registerGlobal(void* data, wl_registry* reg, uint32_t name, const char* intf, uint32_t v);
	static void registerGlobalRemove(void* data, wl_registry* reg, uint32_t name);

	static void seatCap(void* data, wl_seat* seat, uint32_t cap);
	static void seatName(void* data, wl_seat* seat, const char* name);

	static void keyboardKey(void* data, wl_keyboard* kb, uint32_t ser, uint32_t t, uint32_t key, uint32_t stat);
	static void pointerButton(void* data, wl_pointer* pointer, uint32_t serial, uint32_t time, uint32_t button, uint32_t state);
	static void pointerEnter(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface, wl_fixed_t x, wl_fixed_t yInt32);
	static void pointerExit(void* data, wl_pointer* pointer, uint32_t serial, wl_surface* surface);

	friend GUIToolkit;
};
