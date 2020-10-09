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
#include <SFML/Window/Unix/ClipboardImpl.hpp>
#include <SFML/Window/Unix/Display.hpp>
#include <SFML/Window/Unix/Wayland/ClipboardImplWayland.hpp>
#include <SFML/Window/Unix/X11/ClipboardImplX11.hpp>

namespace sf
{
namespace priv
{
String ClipboardImpl::getString()
{
    String r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = ClipboardImplWayland::getString();
    } else {
        r = ClipboardImplX11::getString();
    }
    unrefDisplay();
    return r;
}


////////////////////////////////////////////////////////////
void ClipboardImpl::setString(const String& text)
{
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        ClipboardImplWayland::setString(text);
    } else {
        ClipboardImplX11::setString(text);
    }
    unrefDisplay();
}


////////////////////////////////////////////////////////////
void ClipboardImpl::processEvents()
{
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        ClipboardImplWayland::processEvents();
    } else {
        ClipboardImplX11::processEvents();
    }
    unrefDisplay();
}

} // namespace priv

} // namespace sf
