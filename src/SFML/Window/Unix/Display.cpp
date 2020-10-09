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
#include <SFML/System/Err.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/Window/Unix/Display.hpp>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <SFML/Window/Unix/X11/DisplayX11.hpp>

#include <cstring>

namespace
{
    // The shared display and its reference counter
    sf::priv::WaylandDisplay* tmpWaylandDisplay = NULL;
    ::Display* tmpX11Display = NULL;
    bool found_display = false;
    sf::priv::DisplayType type = sf::priv::DisplayType::X11;
}

namespace sf
{
namespace priv
{
enum DisplayType getDisplayType() {
    if (found_display) {
        return type;
    }

    bool tryWaylandFirst = false;
    char* sfml_backend = getenv("SFML_BACKEND");
    if (sfml_backend && strcmp(sfml_backend, "wayland") == 0) {
        tryWaylandFirst = true;
    }
    // The only reliable way to determine the display type is to try to connect.
    // (but once connected, we'd like to keep the connection alive)
    // TODO: precheck with WAYLAND_DISPLAY/WAYLAND_SOCKET and DISPLAY
    if (tryWaylandFirst) {
        tmpWaylandDisplay = OpenWaylandDisplay();
        if (tmpWaylandDisplay) {
            type = DisplayType::Wayland;
            found_display = true;
            return type;
        }

        tmpX11Display = OpenX11Display();
        if (tmpX11Display) {
            type = DisplayType::X11;
            found_display = true;
            return type;
        }
        err() << "Could neither connect to a Wayland nor an X11 display. Check if WAYLAND_DISPLAY or X11_DISPLAY is set" << std::endl;
        std::abort();
    } else {
        tmpX11Display = OpenX11Display();
        if (tmpX11Display) {
            type = DisplayType::X11;
            found_display = true;
            return type;
        }

        // Do not try Wayland by default, because backend is unstable

        err() << "Could not connect to an X11 display. Check if X11_DISPLAY is set" << std::endl;
        std::abort();
    }
}

void unrefDisplay() {
    if (tmpWaylandDisplay) {
        CloseWaylandDisplay(tmpWaylandDisplay);
        tmpWaylandDisplay = NULL;
    }
    if (tmpX11Display) {
        CloseX11Display(tmpX11Display);
        tmpX11Display = NULL;
    }
}

} // namespace priv

} // namespace sf
