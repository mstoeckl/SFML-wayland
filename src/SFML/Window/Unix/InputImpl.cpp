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
#include <SFML/Window/Window.hpp> // important to be included first (conflict with None)
#include <SFML/Window/Unix/Wayland/InputImplWayland.hpp>
#include <SFML/Window/Unix/X11/InputImplX11.hpp>
#include <SFML/Window/Unix/InputImpl.hpp>
#include <SFML/Window/Unix/Display.hpp>
#include <SFML/System/Err.hpp>

namespace sf
{
namespace priv
{

////////////////////////////////////////////////////////////
bool InputImpl::isKeyPressed(Keyboard::Key key) {
    bool r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = InputImplWayland::isKeyPressed(key);
    } else {
        r = InputImplX11::isKeyPressed(key);
    }
    unrefDisplay();
    return r;

}

////////////////////////////////////////////////////////////
void InputImpl::setVirtualKeyboardVisible(bool visible) {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        InputImplWayland::setVirtualKeyboardVisible(visible);
    } else {
        InputImplX11::setVirtualKeyboardVisible(visible);
    }
    unrefDisplay();
}

////////////////////////////////////////////////////////////
bool InputImpl::isMouseButtonPressed(Mouse::Button button) {
    bool r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = InputImplWayland::isMouseButtonPressed(button);
    } else {
        r = InputImplX11::isMouseButtonPressed(button);
    }
    unrefDisplay();
    return r;
}

////////////////////////////////////////////////////////////
Vector2i InputImpl::getMousePosition() {
    Vector2i r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = InputImplWayland::getMousePosition();
    } else {
        r = InputImplX11::getMousePosition();
    }
    unrefDisplay();
    return r;
}
////////////////////////////////////////////////////////////
Vector2i InputImpl::getMousePosition(const WindowBase& relativeTo) {
    Vector2i r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = InputImplWayland::getMousePosition(relativeTo);
    } else {
        r = InputImplX11::getMousePosition(relativeTo);
    }
    unrefDisplay();
    return r;
}

////////////////////////////////////////////////////////////
void InputImpl::setMousePosition(const Vector2i& position) {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        InputImplWayland::setMousePosition(position);
    } else {
        InputImplX11::setMousePosition(position);
    }
    unrefDisplay();
}

////////////////////////////////////////////////////////////
void InputImpl::setMousePosition(const Vector2i& position, const WindowBase& relativeTo) {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        InputImplWayland::setMousePosition(position, relativeTo);
    } else {
        InputImplX11::setMousePosition(position, relativeTo);
    }
    unrefDisplay();
}

////////////////////////////////////////////////////////////
bool InputImpl::isTouchDown(unsigned int finger) {
    bool r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = InputImplWayland::isTouchDown(finger);
    } else {
        r = InputImplX11::isTouchDown(finger);
    }
    unrefDisplay();
    return r;
}

////////////////////////////////////////////////////////////
Vector2i InputImpl::getTouchPosition(unsigned int finger) {
    Vector2i r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = InputImplWayland::getTouchPosition(finger);
    } else {
        r = InputImplX11::getTouchPosition(finger);
    }
    unrefDisplay();
    return r;
}

////////////////////////////////////////////////////////////
Vector2i InputImpl::getTouchPosition(unsigned int finger, const WindowBase& relativeTo) {
    Vector2i r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = InputImplWayland::getTouchPosition(finger, relativeTo);
    } else {
        r = InputImplX11::getTouchPosition(finger, relativeTo);
    }
    unrefDisplay();
    return r;
}

} // namespace priv

} // namespace sf

