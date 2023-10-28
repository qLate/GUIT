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
