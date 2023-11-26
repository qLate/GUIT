#include <iostream>
#include "GUIToolkit.h"
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

	initEGL();
	initCairo();

}
GUIToolkit::~GUIToolkit()
{
	if (keyboard) wl_keyboard_destroy(keyboard);
	wl_seat_release(seat);
	wl_display_disconnect(display);
}

void GUIToolkit::loop() const
{
	while (wl_display_dispatch(display) && !closeTrigger) { }
}

void GUIToolkit::initCairo()
{
	cairoDevice = cairo_egl_device_create(eglDisplay, eglContext);
}
void GUIToolkit::initEGL() const
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

	static const EGLint context_attributes[] = {
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
