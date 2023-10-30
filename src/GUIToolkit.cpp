#include "GUIToolkit.h"

#include <iostream>

#include "Component.h"
#include "Window.h"

GUIToolkit::GUIToolkit()
{
	if (instance != nullptr) std::cerr << "Multiple GUIToolkit instances detected. More then 1 instance can break the behaviour.";
	instance = this;

	display = wl_display_connect(nullptr);
	wl_registry* registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &listeners.registryListener, this);
	wl_display_roundtrip(display);

	// Cursor
	cursorTheme = wl_cursor_theme_load("Yaru", 24, sharedMemory);
	wl_cursor* cursor = wl_cursor_theme_get_cursor(cursorTheme, "left_ptr");
	cursorImage = cursor->images[0];
	wl_buffer* cursorBuffer = wl_cursor_image_get_buffer(cursorImage);

	cursorSurface = wl_compositor_create_surface(compositor);
	wl_surface_attach(cursorSurface, cursorBuffer, 0, 0);
	wl_surface_commit(cursorSurface);
}
GUIToolkit::~GUIToolkit()
{
	if (keyboard) wl_keyboard_destroy(keyboard);
	wl_seat_release(seat);
	wl_display_disconnect(display);

	for (const auto& window : windows)
	{
		window->destroy();
	}
}

void GUIToolkit::loop() const
{
	while (wl_display_dispatch(display) && !closeTrigger) { }
}
