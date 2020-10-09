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
#include <SFML/Window/Unix/Wayland/ClipboardImplWayland.hpp>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/System/Err.hpp>
#include <vector>

namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
String ClipboardImplWayland::getString()
{
    return getInstance().getStringImpl();
}


////////////////////////////////////////////////////////////
void ClipboardImplWayland::setString(const String& text)
{
    getInstance().setStringImpl(text);
}


////////////////////////////////////////////////////////////
void ClipboardImplWayland::processEvents()
{
    getInstance().processEventsImpl();
}


////////////////////////////////////////////////////////////
ClipboardImplWayland::ClipboardImplWayland() :
//m_window (0),
m_requestResponded(false)
{
    // Open a connection with the X server
    m_display = OpenWaylandDisplay();

}


////////////////////////////////////////////////////////////
ClipboardImplWayland::~ClipboardImplWayland()
{
    CloseWaylandDisplay(m_display);
}


////////////////////////////////////////////////////////////
ClipboardImplWayland& ClipboardImplWayland::getInstance()
{
    static ClipboardImplWayland instance;

    return instance;
}


////////////////////////////////////////////////////////////
String ClipboardImplWayland::getStringImpl()
{
    return String();
}


////////////////////////////////////////////////////////////
void ClipboardImplWayland::setStringImpl(const String& text)
{
}


////////////////////////////////////////////////////////////
void ClipboardImplWayland::processEventsImpl()
{
}

} // namespace priv

} // namespace sf
