#pragma once

#include <vector>
#include <wayland-client-protocol.h>
#include <wayland-cursor.h>

#include "xdg-shell-client-protocol.h"
#include "GUIToolkitListeners.h"
#include "vec2.hpp"
#include "detail/func_packing_simd.inl"
#include "Action.h"

class Component;
class GUIToolkit;
class Window;


class GUIToolkit
{
	bool closeTrigger = false;

	GUIToolkitListeners listeners {};

public:
	inline static constexpr int windowResizeBorder = 10;

	inline static GUIToolkit* instance = nullptr;
	inline static std::vector<Window*> windows {};

	inline static Window* focusedWindow = nullptr;
	inline static Component* focusedComponent = nullptr;
	inline static wl_surface* focusedSurface = nullptr;

	inline static glm::vec2 mousePos;
	inline static int resizeIndex;
	inline static Action<glm::vec2> onPointerMove;
	inline static uint32_t latestPointerEnterSerial;

	inline static wl_surface* cursorSurface;
	inline static wl_subsurface* cursorSubsurface;
	inline static wl_cursor_image* cursorImage;
	inline static wl_cursor_theme* cursorTheme;

	inline static wl_display* display = nullptr;
	inline static wl_compositor* compositor = nullptr;
	inline static wl_subcompositor* subcompositor = nullptr;
	inline static wl_shm* sharedMemory = nullptr;
	inline static xdg_wm_base* sharedBase = nullptr;
	inline static wl_seat* seat = nullptr;
	inline static wl_keyboard* keyboard = nullptr;
	inline static wl_pointer* pointer = nullptr;


	GUIToolkit();
	~GUIToolkit();

	void loop() const;

	friend class GUIToolkitListeners;
};
