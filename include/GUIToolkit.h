#pragma once

#include <vector>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>

#include "GUIToolkitListeners.h"
#include "vec2.hpp"
#include "detail/func_packing_simd.inl"
#include "Action.h"

#include <wayland-client.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>

#include "xdg-shell-client-protocol.h"

class Component;
class GUIToolkit;
class WindowW;


class GUIToolkit
{
	bool closeTrigger = false;

	GUIToolkitListeners listeners {};

public:
	inline static constexpr int windowResizeBorder = 10;

	inline static GUIToolkit* instance = nullptr;
	inline static std::vector<WindowW*> windows {};

	// Hovered and focused components
	inline static WindowW* hoveredWindow = nullptr;
	inline static Component* hoveredComponent = nullptr;
	inline static wl_surface* hoveredSurface = nullptr;
	inline static WindowW* focusedWindow = nullptr;
	inline static Component* focusedComponent = nullptr;

	// Mouse
	inline static glm::vec2 mousePos;
	inline static int resizeIndex;
	inline static Action<glm::vec2> onPointerMove;
	inline static uint32_t latestPointerEnterSerial;

	// Wayland cursor
	inline static wl_surface* cursorSurface;
	inline static wl_subsurface* cursorSubsurface;
	inline static wl_cursor_image* cursorImage;
	inline static wl_cursor_theme* cursorTheme;

	// Wayland
	inline static wl_display* display = nullptr;
	inline static wl_compositor* compositor = nullptr;
	inline static wl_subcompositor* subcompositor = nullptr;
	inline static wl_shm* sharedMemory = nullptr;
	inline static xdg_wm_base* sharedBase = nullptr;
	inline static wl_seat* seat = nullptr;
	inline static wl_keyboard* keyboard = nullptr;
	inline static wl_pointer* pointer = nullptr;

    // EGL Cairo
    inline static EGLDisplay* eglDisplay = nullptr;
    inline static EGLConfig* eglConfig = nullptr;
    inline static EGLContext* eglContext = nullptr;
    inline static cairo_device_t* cairoDevice = nullptr;



	GUIToolkit();
	~GUIToolkit();

	void loop() const;

	static void initCairo();
    void initEGL() const;

    friend class GUIToolkitListeners;
};
