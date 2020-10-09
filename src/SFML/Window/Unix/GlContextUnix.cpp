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

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Unix/GlContextUnix.hpp>
#include <SFML/Window/Unix/Display.hpp>
#include <SFML/Window/Unix/X11/GlContextX11.hpp>
#include <SFML/Window/Unix/Wayland/GlContextWayland.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <vector>

namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
GlContextUnix::GlContextUnix(GlContextUnix* shared) {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        m_wayland = new GlContextWayland(shared ? shared->m_wayland : NULL);
    } else {
        m_x11 = new GlxContext(shared ? shared->m_x11 : NULL);
    }
    unrefDisplay();
}

////////////////////////////////////////////////////////////
GlContextUnix::GlContextUnix(GlContextUnix* shared, const ContextSettings& settings, const WindowImpl* owner, unsigned int bitsPerPixel) {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        m_wayland = new GlContextWayland(shared ? shared->m_wayland : NULL, settings, owner, bitsPerPixel);
    } else {
        m_x11 = new GlxContext(shared ? shared->m_x11 : NULL, settings, owner, bitsPerPixel);
    }
    unrefDisplay();
}

////////////////////////////////////////////////////////////
GlContextUnix::GlContextUnix(GlContextUnix* shared, const ContextSettings& settings, unsigned int width, unsigned int height) {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        m_wayland = new GlContextWayland(shared ? shared->m_wayland : NULL, settings, width, height);
    } else {
        m_x11 = new GlxContext(shared ? shared->m_x11 : NULL, settings, width, height);
    }
    unrefDisplay();
}

////////////////////////////////////////////////////////////
GlContextUnix::~GlContextUnix() {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        delete m_wayland;
    } else if (displayType == X11) {
        delete m_x11;
    }
}

////////////////////////////////////////////////////////////
GlFunctionPointer GlContextUnix::getFunction(const char* name) {
    GlFunctionPointer r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = GlContextWayland::getFunction(name);
    } else {
        r = GlxContext::getFunction(name);
    }
    unrefDisplay();
    return r;
}

////////////////////////////////////////////////////////////
/// \brief Activate the context as the current target for rendering
///
/// \param current Whether to make the context current or no longer current
///
/// \return True on success, false if any error happened
///
////////////////////////////////////////////////////////////
bool GlContextUnix::makeCurrent(bool current) {
    bool r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = m_wayland->makeCurrent(current);
    } else {
        r = m_x11->makeCurrent(current);
    }
    return r;
}

////////////////////////////////////////////////////////////
/// \brief Display what has been rendered to the context so far
///
////////////////////////////////////////////////////////////
void GlContextUnix::display() {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        m_wayland->display();
    } else {
        m_x11->display();
    }
}

////////////////////////////////////////////////////////////
/// \brief Enable or disable vertical synchronization
///
/// Activating vertical synchronization will limit the number
/// of frames displayed to the refresh rate of the monitor.
/// This can avoid some visual artifacts, and limit the framerate
/// to a good value (but not constant across different computers).
///
/// \param enabled True to enable v-sync, false to deactivate
///
////////////////////////////////////////////////////////////
void GlContextUnix::setVerticalSyncEnabled(bool enabled) {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        m_wayland->setVerticalSyncEnabled(enabled);
    } else {
        m_x11->setVerticalSyncEnabled(enabled);
    }
}


} // namespace priv

} // namespace sf
