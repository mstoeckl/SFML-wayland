///////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2020 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////

#include <SFML/Window/Unix/Wayland/WindowImplWayland.hpp> // must include first
#include <SFML/System/Err.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <EGL/egl.h>

#include <cstring>
#include <cassert>
#include <cstdlib>
#include <map>
#include <vector>

#include <GLES2/gl2.h>
#include <EGL/eglext.h>
#include <unistd.h>
#include <fcntl.h>

namespace
{
    // The shared display and its reference counter
    static sf::priv::WaylandDisplay shared_globals;
    unsigned int referenceCount = 0;
    sf::Mutex mutex;

    static void
    wm_base_ping(void *data, struct xdg_wm_base *xdg, uint32_t serial)
    {
        xdg_wm_base_pong(xdg, serial);
    }

    static const struct xdg_wm_base_listener wm_base_listener = {
        wm_base_ping
    };

    sf::priv::WindowImplWayland* window_for_surface(struct wl_surface *surface) {
        for (sf::priv::WindowImplWayland* w : shared_globals.window_list) {
            if (w->m_surface == surface) {
                return w;
            }
        }
        return NULL;
    }

    void pointer_enter(void *data,
              struct wl_pointer *wl_pointer,
              uint32_t serial,
              struct wl_surface *surface,
              wl_fixed_t surface_x,
                  wl_fixed_t surface_y) {
        sf::priv::WindowImplWayland* window = window_for_surface(surface);
        if (window) {
            window->handleWaylandPointerEnter();
            shared_globals.pointer_focus_window = window;
        }
    }
    void pointer_leave(void *data,
              struct wl_pointer *wl_pointer,
              uint32_t serial,
                  struct wl_surface *surface) {
        sf::priv::WindowImplWayland* window = window_for_surface(surface);
        if (window) {
            window->handleWaylandPointerLeave();
            shared_globals.pointer_focus_window = NULL;
        }
    }
    void pointer_motion(void *data,
               struct wl_pointer *wl_pointer,
               uint32_t time,
               wl_fixed_t surface_x,
                   wl_fixed_t surface_y) {
        if (shared_globals.pointer_focus_window) {
            double x = wl_fixed_to_double(surface_x);
            double y = wl_fixed_to_double(surface_y);
            shared_globals.pointer_focus_window->handleWaylandPointerMotion(time,x,y);
        }
    }
    void pointer_button(void *data,
               struct wl_pointer *wl_pointer,
               uint32_t serial,
               uint32_t time,
               uint32_t button,
                   uint32_t state) {
        // TODO, call window function adding event to its queue
        if (shared_globals.pointer_focus_window) {
            // TODO: handle serial!
            shared_globals.pointer_focus_window->handleWaylandPointerButton(time, button, state);
        }
    }
    void pointer_axis(void *data,
             struct wl_pointer *wl_pointer,
             uint32_t time,
             uint32_t axis,
                 wl_fixed_t value) {
        if (shared_globals.pointer_focus_window) {
            double val = wl_fixed_to_double(value);
            shared_globals.pointer_focus_window->handleWaylandPointerAxis(time, axis, val);
        }

    }
    void pointer_frame(void *data,
                       struct wl_pointer *wl_pointer) {
        // TODO: is it worth it to coalesce all the pointer info?
        // (Probably yes, so that we can apply the mouse motion events _first_?)
    }
    void pointer_axis_source(void *data,
                struct wl_pointer *wl_pointer,
                        uint32_t axis_source) {

    }
    void pointer_axis_stop(void *data,
              struct wl_pointer *wl_pointer,
              uint32_t time,
                      uint32_t axis) {

    }
    void pointer_axis_discrete(void *data,
                  struct wl_pointer *wl_pointer,
                  uint32_t axis,
                          int32_t discrete) {
        if (shared_globals.pointer_focus_window) {
            shared_globals.pointer_focus_window->handleWaylandPointerAxisDiscrete(axis, discrete);
        }
    }
    static const struct wl_pointer_listener pointer_listener = {
        pointer_enter,
        pointer_leave,
        pointer_motion,
        pointer_button,pointer_axis,
        pointer_frame,pointer_axis_source,
        pointer_axis_stop,pointer_axis_discrete
    };
    void keyboard_keymap(void *data,
               struct wl_keyboard *wl_keyboard,
               uint32_t format,
               int32_t fd,
                         uint32_t size) {
        if (format != WL_KEYBOARD_KEYMAP_FORMAT_XKB_V1) {
            sf::err() << "Bad keymap format" << std::endl;
            std::abort();
        }

#if 0
        FILE *file = fdopen(fd, "r+");
        shared_globals.keyboard_keymap =
                xkb_keymap_new_from_file(shared_globals.keyboard_context,file, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        fclose(file);
#else
        char* buf = new char[size + 1];
        buf[size] = 0;
        if (read(fd, buf, size) != size) {
            sf::err() << "Failed to read keymap" << std::endl;
            std::abort();
        }
        close(fd);
        // xkb_keymap_new_from_buffer crashes, xkb_keymap_new_from_string doesn't ???
//        shared_globals.keyboard_keymap =
//                xkb_keymap_new_from_buffer(shared_globals.keyboard_context,buf,size, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        shared_globals.keyboard_keymap =
                xkb_keymap_new_from_string(shared_globals.keyboard_context,buf, XKB_KEYMAP_FORMAT_TEXT_V1, XKB_KEYMAP_COMPILE_NO_FLAGS);
        delete [] buf;
#endif
        shared_globals.keyboard_state = xkb_state_new(shared_globals.keyboard_keymap);

    }
    void keyboard_enter(void *data,
              struct wl_keyboard *wl_keyboard,
              uint32_t serial,
              struct wl_surface *surface,
                        struct wl_array *keys) {
        sf::priv::WindowImplWayland* window = window_for_surface(surface);
        if (window) {
            window->handleWaylandKeyboardEnter();
            shared_globals.keyboard_focus_window = window;
        }
    }
    void keyboard_leave(void *data,
              struct wl_keyboard *wl_keyboard,
              uint32_t serial,
                        struct wl_surface *surface) {
        sf::priv::WindowImplWayland* window = window_for_surface(surface);
        if (window) {
            window->handleWaylandKeyboardLeave();
            shared_globals.keyboard_focus_window = NULL;
        }
    }
    void keyboard_key(void *data,
            struct wl_keyboard *wl_keyboard,
            uint32_t serial,
            uint32_t time,
            uint32_t key,
                      uint32_t state) {
        if (!shared_globals.keyboard_state) {
            std::abort();
        }
        uint32_t keycode = key + 8;
        if (shared_globals.keyboard_focus_window) {
            // TODO: do we need multikey handling?
            xkb_keysym_t keysym = xkb_state_key_get_one_sym(shared_globals.keyboard_state, keycode);

            if (state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                shared_globals.keyboard_held_keys.push_back(std::pair<uint32_t,uint32_t>(keysym, keycode));
            } else {
                // todo: filter operation? what if the same keycode is pressed twice?
                bool clear = false;
                while (!clear) {
                    clear = true;
                    for (int i=0;i<shared_globals.keyboard_held_keys.size();i++) {
                        if (shared_globals.keyboard_held_keys[i].second == keycode) {
                            shared_globals.keyboard_held_keys.erase(shared_globals.keyboard_held_keys.begin() + i);
                            clear = false;
                            break;
                        }
                    }
                }
            }
            // It looks like we only need keyboard state tracking to figure out
            // our modifiers; otherwise, we just use keycodes
            xkb_mod_mask_t modmask = xkb_state_serialize_mods(shared_globals.keyboard_state, XKB_STATE_MODS_EFFECTIVE);
            shared_globals.keyboard_focus_window->handleWaylandKeyboardKey(time, keysym, state,
                                                                           // TODO: does xkb provide nice names for shift/ctrl/alt/penguinkey?
                                                                           modmask & (1<<1),
                                                                           modmask & (1<<3),
                                                                           modmask & (1<<4),
                                                                           modmask & (1<<6));
            uint32_t unikey= xkb_state_key_get_utf32(shared_globals.keyboard_state, keycode);
            if (unikey && state == WL_KEYBOARD_KEY_STATE_PRESSED) {
                shared_globals.keyboard_focus_window->handleWaylandKeyboardText(unikey);
            }
        }

    }
    void keyboard_modifiers(void *data,
              struct wl_keyboard *wl_keyboard,
              uint32_t serial,
              uint32_t mods_depressed,
              uint32_t mods_latched,
              uint32_t mods_locked,
              uint32_t group) {
        // TODO handle serial
        if (!shared_globals.keyboard_state) {
            std::abort();
        }
        xkb_state_update_mask(shared_globals.keyboard_state, mods_depressed, mods_latched,
                                  mods_locked, 0, 0, group);
    }

    void keyboard_repeat_info(void *data,
                struct wl_keyboard *wl_keyboard,
                int32_t rate,
                              int32_t delay) {

    }


    static const struct wl_keyboard_listener keyboard_listener = {
        keyboard_keymap,
        keyboard_enter,
        keyboard_leave,
        keyboard_key,
        keyboard_modifiers,
        keyboard_repeat_info
    };

    static void
    seat_capabilities(void *data, struct wl_seat *wl_seat,
                      uint32_t capabilities)
    {
        if (!(capabilities & WL_SEAT_CAPABILITY_KEYBOARD)) {
            sf::err() << "We have no keyboard" << std::endl;
            std::abort();
        }
        if (!(capabilities & WL_SEAT_CAPABILITY_POINTER)) {
            sf::err() << "We have no pointer" << std::endl;
            std::abort();
        }
        shared_globals.pointer = wl_seat_get_pointer(shared_globals.seat);
        shared_globals.keyboard = wl_seat_get_keyboard(shared_globals.seat);
        wl_pointer_add_listener(shared_globals.pointer,&pointer_listener, NULL);
        wl_keyboard_add_listener(shared_globals.keyboard,&keyboard_listener, NULL);
    }
    void seat_name(void *data,struct wl_seat *wl_seat,const char *name)
    {
    }

    static const struct wl_seat_listener seat_listener = {
       seat_capabilities,
       seat_name
    };

    void registry_global(void *data,
               struct wl_registry *wl_registry,
               uint32_t name,
               const char *interface,
                   uint32_t version) {
        std::string iface(interface);
        if (iface == "wl_shm") {
            shared_globals.shm = (struct wl_shm*)wl_registry_bind(wl_registry, name, &wl_shm_interface, 1);
        } else if (iface == "wl_compositor") {
            shared_globals.compositor = (struct wl_compositor*)wl_registry_bind(wl_registry, name, &wl_compositor_interface, 1);
        } else if (iface == "wl_data_device_manager") {
            shared_globals.data_device_manager = (struct wl_data_device_manager*)wl_registry_bind(wl_registry, name, &wl_data_device_manager_interface, 1);
        } else  if (iface == "xdg_wm_base") {
               shared_globals.xdg_wm_base = (struct xdg_wm_base*)wl_registry_bind(wl_registry, name, &xdg_wm_base_interface, 1);
            xdg_wm_base_add_listener(shared_globals.xdg_wm_base, &wm_base_listener, NULL);
        } else if (iface == "zwp_relative_pointer_manager_v1") {
            shared_globals.relative_pointer_manager = (struct zwp_relative_pointer_manager_v1*)wl_registry_bind(wl_registry, name, &zwp_relative_pointer_manager_v1_interface, 1);
        } else if (iface == "zwp_pointer_constraints_v1") {
            shared_globals.pointer_constraints = (struct zwp_pointer_constraints_v1*)wl_registry_bind(wl_registry, name, &zwp_pointer_constraints_v1_interface, 1);
        } else if (iface == "zxdg_decoration_manager_v1") {
            shared_globals.decoration_manager = (struct zxdg_decoration_manager_v1*)wl_registry_bind(wl_registry, name, &zxdg_decoration_manager_v1_interface, 1);
        }else if (iface == "wl_seat") {
            shared_globals.seat = (struct wl_seat*)wl_registry_bind(wl_registry, name, &wl_seat_interface, 7);
            wl_seat_add_listener(shared_globals.seat, &seat_listener, NULL);
        }else if (iface == "wl_output") {
            shared_globals.output = (struct wl_output*)wl_registry_bind(wl_registry, name, &wl_output_interface, 1);
        }
    }
    void registry_global_remove(void *data,
                  struct wl_registry *wl_registry,
                                uint32_t name) {

    }

    const struct wl_registry_listener registry_listener {
        registry_global, registry_global_remove
    };


}

namespace sf
{
namespace priv
{
WaylandDisplay::WaylandDisplay() :
    registry(NULL),
    shm(NULL),
    compositor(NULL),
    data_device_manager(NULL),
    xdg_wm_base(NULL),
    relative_pointer_manager(NULL),
    pointer_constraints(NULL),
    decoration_manager(NULL),
    egl_dpy(EGL_NO_DISPLAY),
    seat(NULL),
    output(NULL),
    window_list()
{

    pointer = NULL;
    keyboard = NULL;
    keyboard_focus_window = NULL;
    pointer_focus_window = NULL;
    keyboard_keymap = NULL;
    keyboard_state = NULL;

}

////////////////////////////////////////////////////////////
sf::priv::WaylandDisplay* OpenWaylandDisplay()
{
    Lock lock(mutex);

    if (referenceCount == 0)
    {
        shared_globals.display = wl_display_connect(NULL);

        // Opening display failed: The best we can do at the moment is to output a meaningful error message
        // and cause an abnormal program termination
        if (!shared_globals.display)
        {
            err() << "Failed to open Wayland display; make sure the WAYLAND_DISPLAY environment variable is set correctly" << std::endl;
            std::abort();
        }
        shared_globals.registry = wl_display_get_registry(shared_globals.display);
        wl_registry_add_listener(shared_globals.registry, &registry_listener, NULL);

        wl_display_roundtrip(shared_globals.display);

        if (!shared_globals.shm) {
            err() << "Wayland compositor does not support wl_shm" << std::endl;
            std::abort();
        } else if (!shared_globals.compositor) {
            err() << "Wayland compositor does not support wl_compositor" << std::endl;
            std::abort();
        }else if (!shared_globals.data_device_manager) {
            err() << "Wayland compositor does not support wl_data_device_manager" << std::endl;
            std::abort();
        }else if (!shared_globals.xdg_wm_base) {
            err() << "Wayland compositor does not support xdg_wm_base" << std::endl;
            std::abort();
        }else if (!shared_globals.relative_pointer_manager) {
            err() << "Wayland compositor does not support zwp_relative_pointer_manager_v1" << std::endl;
            std::abort();
        }else if (!shared_globals.pointer_constraints) {
            err() << "Wayland compositor does not support zwp_pointer_constraints_v1" << std::endl;
            std::abort();
        }else if (!shared_globals.decoration_manager) {
            err() << "Wayland compositor does not support zxdg_decoration_manager_v1" << std::endl;
            std::abort();
        }else if (!shared_globals.seat) {
            err() << "Wayland compositor does have a seat" << std::endl;
            std::abort();
        }else if (!shared_globals.output) {
            err() << "Wayland compositor does have an output" << std::endl;
            std::abort();
        }


        shared_globals.keyboard_context = xkb_context_new(XKB_CONTEXT_NO_FLAGS);


        // TODO: find a decent source of documentation here

        // or OPENGL_ES_API ?
        if (eglBindAPI(EGL_OPENGL_API) == EGL_FALSE) {
            err() << "We don't have EGL" << std::endl;
            std::abort();
        }

        //  TODO: INTEGRATE WITH ALL THE ACTUAL EGL STUFF

        const char *extensions_list = eglQueryString(EGL_NO_DISPLAY, EGL_EXTENSIONS);
        bool has_ext_platform = false, has_khr_platform=false;
        if (strstr(extensions_list, "EGL_EXT_platform_wayland")) {
            has_ext_platform = true;
        }
        if (strstr(extensions_list, "EGL_KHR_platform_wayland")) {
            has_khr_platform = true;
        }
        if (!has_khr_platform && !has_ext_platform) {
            err() << "EGL doesn't seem to support Wayland" << std::endl;
            std::abort();
        }
        // This is a space-separated list of extensions. To verify



        shared_globals.egl_dpy = eglGetPlatformDisplay(has_ext_platform ? EGL_PLATFORM_WAYLAND_EXT :EGL_PLATFORM_WAYLAND_KHR, shared_globals.display, NULL);
        if (shared_globals.egl_dpy == EGL_NO_DISPLAY) {
            err() << "EGL could not make use of the display" << std::endl;
            std::abort();
        }

        int major_version = -1, minor_version = -1;
        EGLBoolean ret = eglInitialize(shared_globals.egl_dpy, &major_version, &minor_version);
        if (ret == EGL_FALSE) {
            err() << "Failed to initialize EGL" << std::endl;
            std::abort();
        }
        err() << major_version << "  " << minor_version<<std::endl;

        wl_display_flush(shared_globals.display); // voodoo, do we need it
    }

    referenceCount++;
    return &shared_globals;
}


////////////////////////////////////////////////////////////
void CloseWaylandDisplay(sf::priv::WaylandDisplay* globals)
{
    Lock lock(mutex);

    assert(globals == &shared_globals);

    referenceCount--;
    if (referenceCount == 0) {
        wl_display_disconnect(globals->display);
    }
}

} // namespace priv

} // namespace sf
