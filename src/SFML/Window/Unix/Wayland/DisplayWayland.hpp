////////////////////////////////////////////////////////////
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

#ifndef SFML_DISPLAY_WAYLAND_HPP
#define SFML_DISPLAY_WAYLAND_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <string>

#include <wayland-client-core.h>
#include <wayland-client-protocol.h>
#include <xdg-shell.h>
#include <relative-pointer-unstable-v1.h>
#include <pointer-constraints-unstable-v1.h>
#include <xdg-decoration-unstable-v1.h>

#include <wayland-egl.h>
#include <vector>
#include <xkbcommon/xkbcommon.h>

namespace sf
{
namespace priv
{
class WindowImplWayland;
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;
class WaylandDisplay {
public:
    WaylandDisplay();
    // TODO: mutex protection for all this, because different windows may run
    // on different threads
    struct wl_display *display;
    struct wl_registry *registry;
    struct wl_shm *shm;
    struct wl_compositor *compositor;
    struct wl_data_device_manager *data_device_manager;
    struct xdg_wm_base *xdg_wm_base;
    struct zwp_relative_pointer_manager_v1 *relative_pointer_manager;
    struct zwp_pointer_constraints_v1 *pointer_constraints;
    struct zxdg_decoration_manager_v1 *decoration_manager;
    EGLDisplay egl_dpy;
    struct xkb_context* keyboard_context;

    // TODO: make vectors of these!
    // (And analogous classes WaylandSeat (incl seat,pointer,keyboard), WaylandOutput)
    struct wl_seat *seat;
    struct wl_pointer* pointer;
    struct wl_keyboard* keyboard;
    std::vector<std::pair<uint32_t, uint32_t>> keyboard_held_keys;
    WindowImplWayland * keyboard_focus_window;
    WindowImplWayland * pointer_focus_window;
    struct xkb_keymap* keyboard_keymap;
    struct xkb_state* keyboard_state;

    struct wl_output *output;
    std::vector<WindowImplWayland *> window_list;
};

////////////////////////////////////////////////////////////
/// \brief Get the shared Display
///
/// This function increments the reference count of the display,
/// it must be matched with a call to CloseDisplay.
///
/// \return Pointer to the shared display
///
////////////////////////////////////////////////////////////
WaylandDisplay* OpenWaylandDisplay();

////////////////////////////////////////////////////////////
/// \brief Release a reference to the shared display
///
/// \param display Display to release
///
////////////////////////////////////////////////////////////
void CloseWaylandDisplay(WaylandDisplay* display);

} // namespace priv

} // namespace sf


#endif // SFML_DISPLAY_WAYLAND_HPP


