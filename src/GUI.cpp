// ReSharper disable CppMemberFunctionMayBeStatic
#include "GUI.h"

#include <iostream>
#include <linux/input-event-codes.h>

#include "SubComponent.h"
#include "Window.h"


GUI::GUI()
{
	if (instance != nullptr) std::cerr << "Multiple GUI Toolkit instances detected. More then 1 instance will probably break the behaviour.";
	instance = this;

	initWayland();
	initCursor();
	initEGL();
	initCairo();

	initInteraction();
}
void GUI::initWayland()
{
	display = wl_display_connect(nullptr);
	wl_registry* registry = wl_display_get_registry(display);
	wl_registry_add_listener(registry, &listeners.registryListener, this);
	wl_display_roundtrip(display);
}
void GUI::initCursor()
{
	cursorTheme = wl_cursor_theme_load("Yaru", 24, sharedMemory);
	wl_cursor* cursor = wl_cursor_theme_get_cursor(cursorTheme, "left_ptr");
	cursorImage = cursor->images[0];
	wl_buffer* cursorBuffer = wl_cursor_image_get_buffer(cursorImage);

	cursorSurface = wl_compositor_create_surface(compositor);
	wl_surface_attach(cursorSurface, cursorBuffer, 0, 0);
	wl_surface_commit(cursorSurface);
}
void GUI::initCairo()
{
	cairoDevice = cairo_egl_device_create(eglDisplay, eglContext);
}
void GUI::initEGL() const
{
	EGLint major, minor, count, n, size;
	EGLint config_attributes[] = {
		EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
		EGL_RED_SIZE, 8,
		EGL_GREEN_SIZE, 8,
		EGL_BLUE_SIZE, 8,
		EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
		EGL_NONE
	};

	static constexpr EGLint context_attributes[] = {
		EGL_CONTEXT_CLIENT_VERSION, 2,
		EGL_NONE
	};

	eglDisplay = static_cast<EGLDisplay*>(eglGetDisplay(display));
	eglInitialize(eglDisplay, &major, &minor);
	eglBindAPI(EGL_OPENGL_API);

	eglGetConfigs(eglDisplay, NULL, 0, &count);

	EGLConfig* configs = static_cast<EGLConfig*>(calloc(count, sizeof *configs));
	eglChooseConfig(eglDisplay, config_attributes, configs, count, &n);

	for (int i = 0; i < n; i++)
	{
		eglGetConfigAttrib(eglDisplay, configs[i], EGL_BUFFER_SIZE, &size);
		eglGetConfigAttrib(eglDisplay, configs[i], EGL_RED_SIZE, &size);

		eglConfig = static_cast<EGLConfig*>(configs[i]);
		break;
	}

	eglContext = static_cast<EGLContext*>(eglCreateContext(eglDisplay, eglConfig, EGL_NO_CONTEXT, context_attributes));
}
void GUI::initInteraction()
{
	onPointerDown += [](int button, int serial)
	{
		if (button != BTN_LEFT) return;
		if (focusedWindow->isResizeable && resizeIndex != 0)
		{
			xdg_toplevel_resize(focusedWindow->top, seat, serial, resizeIndex);
		}
		else if (hoveredSurface == hoveredWindow->header->surf &&
			mousePos.y <= WindoW::headerHeight + WindoW::resizeBorder && mousePos.y > WindoW::resizeBorder &&
			mousePos.x > WindoW::resizeBorder && mousePos.x < focusedWindow->wSize.x - WindoW::resizeBorder)
		{
			xdg_toplevel_move(focusedWindow->top, seat, serial);
		}
	};
}

GUI::~GUI()
{
	if (keyboard) wl_keyboard_destroy(keyboard);
	wl_seat_release(seat);
	wl_display_disconnect(display);
}

void GUI::loop() const
{
	while (wl_display_dispatch(display) && !closeTrigger)
	{
		
	}
}
