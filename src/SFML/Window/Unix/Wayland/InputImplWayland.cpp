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
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <SFML/System/Err.hpp>

#include <xkbcommon/xkbcommon.h>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
bool InputImplWayland::isKeyPressed(Keyboard::Key key)
{
    // Sanity checks
    if (key < 0 || key >= sf::Keyboard::KeyCount)
        return false;

    // Get the corresponding X11 keysym
    uint32_t keysym = 0;
    switch (key)
    {
        case Keyboard::LShift:     keysym = XKB_KEY_Shift_L;      break;
        case Keyboard::RShift:     keysym = XKB_KEY_Shift_R;      break;
        case Keyboard::LControl:   keysym = XKB_KEY_Control_L;    break;
        case Keyboard::RControl:   keysym = XKB_KEY_Control_R;    break;
        case Keyboard::LAlt:       keysym = XKB_KEY_Alt_L;        break;
        case Keyboard::RAlt:       keysym = XKB_KEY_Alt_R;        break;
        case Keyboard::LSystem:    keysym = XKB_KEY_Super_L;      break;
        case Keyboard::RSystem:    keysym = XKB_KEY_Super_R;      break;
        case Keyboard::Menu:       keysym = XKB_KEY_Menu;         break;
        case Keyboard::Escape:     keysym = XKB_KEY_Escape;       break;
        case Keyboard::Semicolon:  keysym = XKB_KEY_semicolon;    break;
        case Keyboard::Slash:      keysym = XKB_KEY_slash;        break;
        case Keyboard::Equal:      keysym = XKB_KEY_equal;        break;
        case Keyboard::Hyphen:     keysym = XKB_KEY_minus;        break;
        case Keyboard::LBracket:   keysym = XKB_KEY_bracketleft;  break;
        case Keyboard::RBracket:   keysym = XKB_KEY_bracketright; break;
        case Keyboard::Comma:      keysym = XKB_KEY_comma;        break;
        case Keyboard::Period:     keysym = XKB_KEY_period;       break;
        case Keyboard::Quote:      keysym = XKB_KEY_apostrophe;   break;
        case Keyboard::Backslash:  keysym = XKB_KEY_backslash;    break;
        case Keyboard::Tilde:      keysym = XKB_KEY_grave;        break;
        case Keyboard::Space:      keysym = XKB_KEY_space;        break;
        case Keyboard::Enter:      keysym = XKB_KEY_Return;       break;
        case Keyboard::Backspace:  keysym = XKB_KEY_BackSpace;    break;
        case Keyboard::Tab:        keysym = XKB_KEY_Tab;          break;
        case Keyboard::PageUp:     keysym = XKB_KEY_Prior;        break;
        case Keyboard::PageDown:   keysym = XKB_KEY_Next;         break;
        case Keyboard::End:        keysym = XKB_KEY_End;          break;
        case Keyboard::Home:       keysym = XKB_KEY_Home;         break;
        case Keyboard::Insert:     keysym = XKB_KEY_Insert;       break;
        case Keyboard::Delete:     keysym = XKB_KEY_Delete;       break;
        case Keyboard::Add:        keysym = XKB_KEY_KP_Add;       break;
        case Keyboard::Subtract:   keysym = XKB_KEY_KP_Subtract;  break;
        case Keyboard::Multiply:   keysym = XKB_KEY_KP_Multiply;  break;
        case Keyboard::Divide:     keysym = XKB_KEY_KP_Divide;    break;
        case Keyboard::Pause:      keysym = XKB_KEY_Pause;        break;
        case Keyboard::F1:         keysym = XKB_KEY_F1;           break;
        case Keyboard::F2:         keysym = XKB_KEY_F2;           break;
        case Keyboard::F3:         keysym = XKB_KEY_F3;           break;
        case Keyboard::F4:         keysym = XKB_KEY_F4;           break;
        case Keyboard::F5:         keysym = XKB_KEY_F5;           break;
        case Keyboard::F6:         keysym = XKB_KEY_F6;           break;
        case Keyboard::F7:         keysym = XKB_KEY_F7;           break;
        case Keyboard::F8:         keysym = XKB_KEY_F8;           break;
        case Keyboard::F9:         keysym = XKB_KEY_F9;           break;
        case Keyboard::F10:        keysym = XKB_KEY_F10;          break;
        case Keyboard::F11:        keysym = XKB_KEY_F11;          break;
        case Keyboard::F12:        keysym = XKB_KEY_F12;          break;
        case Keyboard::F13:        keysym = XKB_KEY_F13;          break;
        case Keyboard::F14:        keysym = XKB_KEY_F14;          break;
        case Keyboard::F15:        keysym = XKB_KEY_F15;          break;
        case Keyboard::Left:       keysym = XKB_KEY_Left;         break;
        case Keyboard::Right:      keysym = XKB_KEY_Right;        break;
        case Keyboard::Up:         keysym = XKB_KEY_Up;           break;
        case Keyboard::Down:       keysym = XKB_KEY_Down;         break;
        case Keyboard::Numpad0:    keysym = XKB_KEY_KP_Insert;    break;
        case Keyboard::Numpad1:    keysym = XKB_KEY_KP_End;       break;
        case Keyboard::Numpad2:    keysym = XKB_KEY_KP_Down;      break;
        case Keyboard::Numpad3:    keysym = XKB_KEY_KP_Page_Down; break;
        case Keyboard::Numpad4:    keysym = XKB_KEY_KP_Left;      break;
        case Keyboard::Numpad5:    keysym = XKB_KEY_KP_Begin;     break;
        case Keyboard::Numpad6:    keysym = XKB_KEY_KP_Right;     break;
        case Keyboard::Numpad7:    keysym = XKB_KEY_KP_Home;      break;
        case Keyboard::Numpad8:    keysym = XKB_KEY_KP_Up;        break;
        case Keyboard::Numpad9:    keysym = XKB_KEY_KP_Page_Up;   break;
        case Keyboard::A:          keysym = XKB_KEY_a;            break;
        case Keyboard::B:          keysym = XKB_KEY_b;            break;
        case Keyboard::C:          keysym = XKB_KEY_c;            break;
        case Keyboard::D:          keysym = XKB_KEY_d;            break;
        case Keyboard::E:          keysym = XKB_KEY_e;            break;
        case Keyboard::F:          keysym = XKB_KEY_f;            break;
        case Keyboard::G:          keysym = XKB_KEY_g;            break;
        case Keyboard::H:          keysym = XKB_KEY_h;            break;
        case Keyboard::I:          keysym = XKB_KEY_i;            break;
        case Keyboard::J:          keysym = XKB_KEY_j;            break;
        case Keyboard::K:          keysym = XKB_KEY_k;            break;
        case Keyboard::L:          keysym = XKB_KEY_l;            break;
        case Keyboard::M:          keysym = XKB_KEY_m;            break;
        case Keyboard::N:          keysym = XKB_KEY_n;            break;
        case Keyboard::O:          keysym = XKB_KEY_o;            break;
        case Keyboard::P:          keysym = XKB_KEY_p;            break;
        case Keyboard::Q:          keysym = XKB_KEY_q;            break;
        case Keyboard::R:          keysym = XKB_KEY_r;            break;
        case Keyboard::S:          keysym = XKB_KEY_s;            break;
        case Keyboard::T:          keysym = XKB_KEY_t;            break;
        case Keyboard::U:          keysym = XKB_KEY_u;            break;
        case Keyboard::V:          keysym = XKB_KEY_v;            break;
        case Keyboard::W:          keysym = XKB_KEY_w;            break;
        case Keyboard::X:          keysym = XKB_KEY_x;            break;
        case Keyboard::Y:          keysym = XKB_KEY_y;            break;
        case Keyboard::Z:          keysym = XKB_KEY_z;            break;
        case Keyboard::Num0:       keysym = XKB_KEY_0;            break;
        case Keyboard::Num1:       keysym = XKB_KEY_1;            break;
        case Keyboard::Num2:       keysym = XKB_KEY_2;            break;
        case Keyboard::Num3:       keysym = XKB_KEY_3;            break;
        case Keyboard::Num4:       keysym = XKB_KEY_4;            break;
        case Keyboard::Num5:       keysym = XKB_KEY_5;            break;
        case Keyboard::Num6:       keysym = XKB_KEY_6;            break;
        case Keyboard::Num7:       keysym = XKB_KEY_7;            break;
        case Keyboard::Num8:       keysym = XKB_KEY_8;            break;
        case Keyboard::Num9:       keysym = XKB_KEY_9;            break;
        default:                   keysym = 0;               break;
    }


    // Open a connection with the Wayland server
    WaylandDisplay* display = OpenWaylandDisplay();
    // NOTE: calling this function is very much not recommended; xkb doesn't seem
    // to have a notion of "held keysym", as the sym going down need not match that
    // going up. Even for pure keycode queries, expect race conditions.
    int nfound = 0;
    for (std::pair<uint32_t,uint32_t> symcode_pair : display->keyboard_held_keys) {
        if (symcode_pair.first == keysym) {
            nfound++;
        }
    }
    CloseWaylandDisplay(display);
    return nfound > 0;
}


////////////////////////////////////////////////////////////
void InputImplWayland::setVirtualKeyboardVisible(bool /*visible*/)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
bool InputImplWayland::isMouseButtonPressed(Mouse::Button button)
{
    return false;
}


////////////////////////////////////////////////////////////
Vector2i InputImplWayland::getMousePosition()
{
    return Vector2i();
}


////////////////////////////////////////////////////////////
Vector2i InputImplWayland::getMousePosition(const WindowBase& relativeTo)
{
}


////////////////////////////////////////////////////////////
void InputImplWayland::setMousePosition(const Vector2i& position)
{
}


////////////////////////////////////////////////////////////
void InputImplWayland::setMousePosition(const Vector2i& position, const WindowBase& relativeTo)
{
}


////////////////////////////////////////////////////////////
bool InputImplWayland::isTouchDown(unsigned int /*finger*/)
{
    // Not applicable
    return false;
}


////////////////////////////////////////////////////////////
Vector2i InputImplWayland::getTouchPosition(unsigned int /*finger*/)
{
    // Not applicable
    return Vector2i();
}


////////////////////////////////////////////////////////////
Vector2i InputImplWayland::getTouchPosition(unsigned int /*finger*/, const WindowBase& /*relativeTo*/)
{
    // Not applicable
    return Vector2i();
}

} // namespace priv

} // namespace sf
