#pragma once

#include <vector>
#include <wayland-client-protocol.h>

#include "xdg-shell-client-protocol.h"
#include "GUIToolkitListeners.h"

class GUIToolkit;
class Window;


class GUIToolkit
{
	bool closeTrigger = false;

	GUIToolkitListeners listeners {};

	wl_display* display = nullptr;
	wl_compositor* compositor = nullptr;
	wl_shm* sharedMemory = nullptr;
	xdg_wm_base* sharedBase = nullptr;
	wl_seat* seat = nullptr;
	wl_keyboard* keyboard = nullptr;
	wl_pointer* pointer = nullptr;

public:
	inline static GUIToolkit* instance = nullptr;
	inline static std::vector<Window*> windows {};
	inline static Window* activeWindow = nullptr;

	GUIToolkit();
	~GUIToolkit();

	void loop() const;

	friend class GUIToolkitListeners;
	friend class Window;
};
