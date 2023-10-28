#pragma once

#include <string>

#include "xdg-shell-client-protocol.h"
#include "wayland-client.h"
#include "WindowListeners.h"
#include "Color.h"

class Window
{
	WindowListeners listeners {};

	wl_surface* surf = nullptr;
	wl_callback* surfCallback = nullptr;
	wl_buffer* buf = nullptr;
	xdg_toplevel* top = nullptr;
	xdg_surface* xSurf = nullptr;

	void updateSurface() const;

public:
	int w = 0, h = 0;
	uint8_t* pixels = nullptr;
	int x = 0, y = 0;
	bool isClosed = false;

	Window(const std::string& name, int w, int h);
	~Window();

	void wClose();

	void resize(int w_, int h_);
	void setPos(int x, int y);

	void fillColor(Color color) const;

	friend class WindowListeners;
	friend class GUIToolkitListeners;
};
