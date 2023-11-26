#include <iostream>
#include "GUIToolkit.h"
#include "WindowW.h"



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
}

void GUIToolkit::loop() const
{
	while (wl_display_dispatch(display) && !closeTrigger) { }
}

void GUIToolkit::init_cairo() {
    cairo_device =
            cairo_egl_device_create(egl_display, egl_context);
    if (cairo_device_status(cairo_device) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "failed to get cairo EGL device\n");
        exit(1);
    }
}


void GUIToolkit::init_egl() {
    EGLint major, minor, count, n, size;
    EGLConfig *configs;
    int i;
    EGLint config_attribs[] = {
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
            EGL_NONE
    };

    static const EGLint context_attribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };


    egl_display = static_cast<EGLDisplay *>(eglGetDisplay((EGLNativeDisplayType) display));
    if (egl_display == EGL_NO_DISPLAY) {
        fprintf(stderr, "Can't create egl display\n");
        exit(1);
    } else {
        fprintf(stderr, "Created egl display\n");
    }

    if (eglInitialize(egl_display, &major, &minor) != EGL_TRUE) {
        fprintf(stderr, "Can't initialise egl display\n");
        exit(1);
    }
    fprintf(stderr, "EGL major: %d, minor %d\n", major, minor);

    if (!eglBindAPI(EGL_OPENGL_API)) {
        fprintf(stderr, "failed to bind EGL client API\n");
    }

    eglGetConfigs(egl_display, NULL, 0, &count);
    printf("EGL has %d configs\n", count);

    configs = static_cast<EGLConfig *>(calloc(count, sizeof *configs));

    eglChooseConfig(egl_display, config_attribs,
                    configs, count, &n);

    for (i = 0; i < n; i++) {
        eglGetConfigAttrib(egl_display,
                           configs[i], EGL_BUFFER_SIZE, &size);
        printf("Buffer size for config %d is %d\n", i, size);
        eglGetConfigAttrib(egl_display,
                           configs[i], EGL_RED_SIZE, &size);
        printf("Red size for config %d is %d\n", i, size);

        // just choose the first one
        egl_conf = static_cast<EGLConfig *>(configs[i]);
        break;
    }

    egl_context =
            static_cast<EGLContext *>(eglCreateContext(egl_display,
                                                       egl_conf,
                                                       EGL_NO_CONTEXT, context_attribs));
    if (egl_context == NULL) {
        fprintf(stderr, "Can't create context\n");
    }
}
