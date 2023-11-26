#define CAIRO
#ifdef CAIRO
#include <stdio.h>

#include <stdlib.h>
#include <string.h>
#include <wayland-client.h>
#include <wayland-client-protocol.h>
#include <wayland-egl.h>
#include <EGL/egl.h>
#include <cairo/cairo-gl.h>
#include "xdg-shell-client-protocol.h"

wl_display *display = NULL;

#include <cairo/cairo-gl.h>

wl_compositor *compositor = NULL;
wl_surface *surf;
wl_egl_window *egl_window;
xdg_wm_base* sharedBase;
EGLDisplay egl_display;

EGLConfig egl_conf;
EGLSurface egl_surface;
EGLContext egl_context;
cairo_surface_t *cairo_surface;

cairo_device_t *cairo_device;
xdg_wm_base_listener sharedBaseListener = {
        .ping = [](void*, xdg_wm_base* sh, uint32_t ser) { xdg_wm_base_pong(sh, ser); }
};

static void global_registry_handler(void *data, struct wl_registry *registry, uint32_t id,
                        const char *interface, uint32_t version)
{
    printf("Got a registry event for %s id %d\n", interface, id);
    if (strcmp(interface, "wl_compositor") == 0) {
        compositor = (wl_compositor *)wl_registry_bind(registry,id,&wl_compositor_interface, 1);
    }
    else if (!strcmp(interface, xdg_wm_base_interface.name))
    {
        sharedBase = (xdg_wm_base*)wl_registry_bind(registry, id, &xdg_wm_base_interface, 1);
        xdg_wm_base_add_listener(sharedBase, &sharedBaseListener, nullptr);
    }
}

static void
global_registry_remover(void *data, struct wl_registry *registry, uint32_t id)
{
    printf("Got a registry losing event for %d\n", id);
}

static const struct wl_registry_listener registry_listener = {
        global_registry_handler,
        global_registry_remover
};

static void init_cairo();

static void
create_window() {

    egl_window = wl_egl_window_create(surf,
                                      480, 360);
    if (egl_window == EGL_NO_SURFACE) {
        fprintf(stderr, "Can't create egl window\n");
        exit(1);
    } else {
        fprintf(stderr, "Created egl window\n");
    }

    egl_surface =
            eglCreateWindowSurface(egl_display,
                                   egl_conf,
                                   egl_window, NULL);

    cairo_surface = cairo_gl_surface_create_for_egl (cairo_device,
                                                     egl_surface,
                                                     480,
                                                     360);

    if (cairo_surface == NULL) {
        fprintf(stderr, "Can't create Cairo surface\n");
        exit(1);
    }  else {
        fprintf(stderr, "Created Cairo surface\n");
    }

    cairo_t *cr = cairo_create(cairo_surface);
    int err;
    if (err = cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Cairo error on create %s\n",
                cairo_status_to_string(static_cast<cairo_status_t>(err)));
    }

    cairo_set_source_rgb(cr, 1.0, 1.0, 0.0);
    if (err = cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Cairo error after set source %s\n",
                cairo_status_to_string(static_cast<cairo_status_t>(err)));
    }
    cairo_paint(cr);

    cairo_set_source_rgb(cr, 0.0, 1.0, 0.0);
    if (err = cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Cairo error after set source %s\n",
                cairo_status_to_string(static_cast<cairo_status_t>(err)));
        exit(1);
    }

    // this is a standard font for Cairo
    cairo_select_font_face (cr, "cairo:serif",
                            CAIRO_FONT_SLANT_NORMAL,
                            CAIRO_FONT_WEIGHT_BOLD);
    cairo_set_font_size (cr, 20);
    cairo_move_to(cr, 10.0, 50.0);
    cairo_show_text (cr, "hello");

    if (err = cairo_status(cr) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "Cairo error after paint %s\n",
                cairo_status_to_string(static_cast<cairo_status_t>(err)));
    }

    cairo_gl_surface_swapbuffers(cairo_surface);
}

static void
init_cairo() {
    cairo_device =
            cairo_egl_device_create(egl_display, egl_context);
    if (cairo_device_status(cairo_device) != CAIRO_STATUS_SUCCESS) {
        fprintf(stderr, "failed to get cairo EGL device\n");
        exit(1);
    }
}

static void
init_egl() {
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


    egl_display = eglGetDisplay((EGLNativeDisplayType) display);
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
        egl_conf = configs[i];
        break;
    }

    egl_context =
            eglCreateContext(egl_display,
                             egl_conf,
                             EGL_NO_CONTEXT, context_attribs);
    if (egl_context == NULL) {
        fprintf(stderr, "Can't create context\n");
    }
}


static void
get_server_references(void) {

    display = wl_display_connect(NULL);
    if (display == NULL) {
        fprintf(stderr, "Can't connect to display\n");
        exit(1);
    }
    printf("connected to display\n");

    struct wl_registry *registry = wl_display_get_registry(display);
    wl_registry_add_listener(registry, &registry_listener, NULL);

    wl_display_dispatch(display);
    wl_display_roundtrip(display);

    if (compositor == NULL ) {
        fprintf(stderr, "Can't find compositor\n");
        exit(1);
    } else {
        fprintf(stderr, "Found compositor\n");
    }
}

int main(int argc, char **argv) {

    get_server_references();

    surf = wl_compositor_create_surface(compositor);
    auto xSurf = xdg_wm_base_get_xdg_surface(sharedBase, surf);

    auto top = xdg_surface_get_toplevel(xSurf);

    init_egl();
    init_cairo();

    create_window();

    while (wl_display_dispatch(display) != -1) {
        ;
    }

    wl_display_disconnect(display);
    printf("disconnected from display\n");

    exit(0);
}


#else
#include "GUIToolkit.h"


#include "Component.h"
#include "WindowW.h"
#include "SubComponent.h"


int main()

{
    GUIToolkit toolkit {};

    WindowW w1 {"Test window", {1, 1}};
    w1.setImage("../images/sviat.jpg");
    w1.resize({500, 500});

    SubComponent s1 {&w1, {w1.size.x/2, 100}};
    s1.moveWindowOnDrag = true;
    s1.setAnchors({0, 0.5f}, {1, 0.5f});
    s1.setColor(Color::blue());


    toolkit.loop();

    return 0;
}
#endif
