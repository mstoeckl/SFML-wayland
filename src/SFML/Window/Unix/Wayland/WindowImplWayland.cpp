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
#include <SFML/Window/Unix/Wayland/WindowImplWayland.hpp>
#include <SFML/Window/Unix/Wayland/ClipboardImplWayland.hpp>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <SFML/Window/Unix/Wayland/InputImplWayland.hpp>
#include <SFML/System/Utf.hpp>
#include <SFML/System/Err.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Sleep.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>

////////////////////////////////////////////////////////////
// Private data
////////////////////////////////////////////////////////////
namespace
{
    sf::priv::WindowImplWayland*              fullscreenWindow = NULL;
    std::vector<sf::priv::WindowImplWayland*> allWindows;
    sf::Mutex                             allWindowsMutex;
    sf::String                            windowManagerName;

    sf::String                            wmAbsPosGood[] = { "Enlightenment", "FVWM", "i3" };

    static const unsigned int             maxTrialsCount = 5;

    sf::Keyboard::Key keysym_to_key(uint32_t symbol)
    {
        switch (symbol)
        {
            case XKB_KEY_Shift_L:      return sf::Keyboard::LShift;
            case XKB_KEY_Shift_R:      return sf::Keyboard::RShift;
            case XKB_KEY_Control_L:    return sf::Keyboard::LControl;
            case XKB_KEY_Control_R:    return sf::Keyboard::RControl;
            case XKB_KEY_Alt_L:        return sf::Keyboard::LAlt;
            case XKB_KEY_Alt_R:        return sf::Keyboard::RAlt;
            case XKB_KEY_Super_L:      return sf::Keyboard::LSystem;
            case XKB_KEY_Super_R:      return sf::Keyboard::RSystem;
            case XKB_KEY_Menu:         return sf::Keyboard::Menu;
            case XKB_KEY_Escape:       return sf::Keyboard::Escape;
            case XKB_KEY_semicolon:    return sf::Keyboard::Semicolon;
            case XKB_KEY_slash:        return sf::Keyboard::Slash;
            case XKB_KEY_equal:        return sf::Keyboard::Equal;
            case XKB_KEY_minus:        return sf::Keyboard::Hyphen;
            case XKB_KEY_bracketleft:  return sf::Keyboard::LBracket;
            case XKB_KEY_bracketright: return sf::Keyboard::RBracket;
            case XKB_KEY_comma:        return sf::Keyboard::Comma;
            case XKB_KEY_period:       return sf::Keyboard::Period;
            case XKB_KEY_apostrophe:   return sf::Keyboard::Quote;
            case XKB_KEY_backslash:    return sf::Keyboard::Backslash;
            case XKB_KEY_grave:        return sf::Keyboard::Tilde;
            case XKB_KEY_space:        return sf::Keyboard::Space;
            case XKB_KEY_Return:       return sf::Keyboard::Enter;
            case XKB_KEY_KP_Enter:     return sf::Keyboard::Enter;
            case XKB_KEY_BackSpace:    return sf::Keyboard::Backspace;
            case XKB_KEY_Tab:          return sf::Keyboard::Tab;
            case XKB_KEY_Prior:        return sf::Keyboard::PageUp;
            case XKB_KEY_Next:         return sf::Keyboard::PageDown;
            case XKB_KEY_End:          return sf::Keyboard::End;
            case XKB_KEY_Home:         return sf::Keyboard::Home;
            case XKB_KEY_Insert:       return sf::Keyboard::Insert;
            case XKB_KEY_Delete:       return sf::Keyboard::Delete;
            case XKB_KEY_KP_Add:       return sf::Keyboard::Add;
            case XKB_KEY_KP_Subtract:  return sf::Keyboard::Subtract;
            case XKB_KEY_KP_Multiply:  return sf::Keyboard::Multiply;
            case XKB_KEY_KP_Divide:    return sf::Keyboard::Divide;
            case XKB_KEY_Pause:        return sf::Keyboard::Pause;
            case XKB_KEY_F1:           return sf::Keyboard::F1;
            case XKB_KEY_F2:           return sf::Keyboard::F2;
            case XKB_KEY_F3:           return sf::Keyboard::F3;
            case XKB_KEY_F4:           return sf::Keyboard::F4;
            case XKB_KEY_F5:           return sf::Keyboard::F5;
            case XKB_KEY_F6:           return sf::Keyboard::F6;
            case XKB_KEY_F7:           return sf::Keyboard::F7;
            case XKB_KEY_F8:           return sf::Keyboard::F8;
            case XKB_KEY_F9:           return sf::Keyboard::F9;
            case XKB_KEY_F10:          return sf::Keyboard::F10;
            case XKB_KEY_F11:          return sf::Keyboard::F11;
            case XKB_KEY_F12:          return sf::Keyboard::F12;
            case XKB_KEY_F13:          return sf::Keyboard::F13;
            case XKB_KEY_F14:          return sf::Keyboard::F14;
            case XKB_KEY_F15:          return sf::Keyboard::F15;
            case XKB_KEY_Left:         return sf::Keyboard::Left;
            case XKB_KEY_Right:        return sf::Keyboard::Right;
            case XKB_KEY_Up:           return sf::Keyboard::Up;
            case XKB_KEY_Down:         return sf::Keyboard::Down;
            case XKB_KEY_KP_Insert:    return sf::Keyboard::Numpad0;
            case XKB_KEY_KP_End:       return sf::Keyboard::Numpad1;
            case XKB_KEY_KP_Down:      return sf::Keyboard::Numpad2;
            case XKB_KEY_KP_Page_Down: return sf::Keyboard::Numpad3;
            case XKB_KEY_KP_Left:      return sf::Keyboard::Numpad4;
            case XKB_KEY_KP_Begin:     return sf::Keyboard::Numpad5;
            case XKB_KEY_KP_Right:     return sf::Keyboard::Numpad6;
            case XKB_KEY_KP_Home:      return sf::Keyboard::Numpad7;
            case XKB_KEY_KP_Up:        return sf::Keyboard::Numpad8;
            case XKB_KEY_KP_Page_Up:   return sf::Keyboard::Numpad9;
            case XKB_KEY_a:            return sf::Keyboard::A;
            case XKB_KEY_b:            return sf::Keyboard::B;
            case XKB_KEY_c:            return sf::Keyboard::C;
            case XKB_KEY_d:            return sf::Keyboard::D;
            case XKB_KEY_e:            return sf::Keyboard::E;
            case XKB_KEY_f:            return sf::Keyboard::F;
            case XKB_KEY_g:            return sf::Keyboard::G;
            case XKB_KEY_h:            return sf::Keyboard::H;
            case XKB_KEY_i:            return sf::Keyboard::I;
            case XKB_KEY_j:            return sf::Keyboard::J;
            case XKB_KEY_k:            return sf::Keyboard::K;
            case XKB_KEY_l:            return sf::Keyboard::L;
            case XKB_KEY_m:            return sf::Keyboard::M;
            case XKB_KEY_n:            return sf::Keyboard::N;
            case XKB_KEY_o:            return sf::Keyboard::O;
            case XKB_KEY_p:            return sf::Keyboard::P;
            case XKB_KEY_q:            return sf::Keyboard::Q;
            case XKB_KEY_r:            return sf::Keyboard::R;
            case XKB_KEY_s:            return sf::Keyboard::S;
            case XKB_KEY_t:            return sf::Keyboard::T;
            case XKB_KEY_u:            return sf::Keyboard::U;
            case XKB_KEY_v:            return sf::Keyboard::V;
            case XKB_KEY_w:            return sf::Keyboard::W;
            case XKB_KEY_x:            return sf::Keyboard::X;
            case XKB_KEY_y:            return sf::Keyboard::Y;
            case XKB_KEY_z:            return sf::Keyboard::Z;
            case XKB_KEY_0:            return sf::Keyboard::Num0;
            case XKB_KEY_1:            return sf::Keyboard::Num1;
            case XKB_KEY_2:            return sf::Keyboard::Num2;
            case XKB_KEY_3:            return sf::Keyboard::Num3;
            case XKB_KEY_4:            return sf::Keyboard::Num4;
            case XKB_KEY_5:            return sf::Keyboard::Num5;
            case XKB_KEY_6:            return sf::Keyboard::Num6;
            case XKB_KEY_7:            return sf::Keyboard::Num7;
            case XKB_KEY_8:            return sf::Keyboard::Num8;
            case XKB_KEY_9:            return sf::Keyboard::Num9;
        }

        return sf::Keyboard::Unknown;
    }
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
WindowImplWayland::WindowImplWayland(WindowHandle handle) :
//m_window         (0),
m_screen         (0),
//m_inputMethod    (NULL),
//m_inputContext   (NULL),
m_isExternal     (true),
m_oldVideoMode   (0),
//m_oldRRCrtc      (0),
//m_hiddenCursor   (0),
//m_lastCursor     (None),
m_keyRepeat      (true),
m_previousSize   (-1, -1),
m_useSizeHints   (false),
m_fullscreen     (false),
m_cursorGrabbed  (false),
m_windowMapped   (false)
//m_iconPixmap     (0),
//m_iconMaskPixmap (0),
//m_lastInputTime  (0)
{
    // Open a connection with the X server
    m_display = OpenWaylandDisplay();

    // TODO
}


void window_xdg_surface_configure(void *data,
          struct xdg_surface *xdg_surface,
                  uint32_t serial) {
    WindowImplWayland* impl = (WindowImplWayland*)data;
    impl->handleWaylandSurfaceConfigure(xdg_surface, serial);
}
void window_xdg_toplevel_configure(void *data,
          struct xdg_toplevel *xdg_toplevel,
          int32_t width,
          int32_t height,
            struct wl_array *states) {

    WindowImplWayland* impl = (WindowImplWayland*)data;
    impl->handleWaylandToplevelConfigure(xdg_toplevel, width, height, states);
}
void window_xdg_toplevel_close(void *data,
                               struct xdg_toplevel *xdg_toplevel) {

    WindowImplWayland* impl = (WindowImplWayland*)data;
    impl->handleWaylandToplevelClose(xdg_toplevel);
}
struct xdg_surface_listener window_xdg_surface_listener {
    window_xdg_surface_configure
};
struct xdg_toplevel_listener window_xdg_toplevel_listener {
    window_xdg_toplevel_configure,
    window_xdg_toplevel_close
};


void WindowImplWayland::handleWaylandSurfaceConfigure(struct xdg_surface *xdg_surface,
                                  uint32_t serial) {
    xdg_surface_ack_configure(xdg_surface, serial);
    err() << "TODO CONFIGURE DETAILS" << std::endl;
    wl_egl_window_resize(m_egl_window, m_window_size.x, m_window_size.y, 0, 0);

    m_xdg_initial_configure_seen = true;
}
void WindowImplWayland::handleWaylandToplevelClose(struct xdg_toplevel *xdg_toplevel) {
    Event evt;
    evt.type = Event::Closed;
    m_new_events.push_back(evt);
}
void WindowImplWayland::handleWaylandToplevelConfigure(
       struct xdg_toplevel *xdg_toplevel,
       int32_t width,
       int32_t height,
        struct wl_array *states) {

        wl_egl_window_resize(m_egl_window,
                     width,
                     height, 0, 0);
}
void WindowImplWayland::handleWaylandKeyboardKey(uint32_t time,uint32_t key,uint32_t state,bool shift, bool control, bool system, bool alt) {
    Event evt;
    evt.type = (state == WL_POINTER_BUTTON_STATE_RELEASED) ? Event::KeyReleased : Event::KeyPressed;
    evt.key.code = keysym_to_key(key);
    evt.key.shift = shift;
    evt.key.control = control;
    evt.key.system = system;
    evt.key.alt = alt;
    m_new_events.push_back(evt);

}
void WindowImplWayland::handleWaylandPointerMotion(uint32_t time,double x, double y) {
    m_current_pointer_position.x = x;
    m_current_pointer_position.y = y;
    Event evt;
    evt.type = Event::MouseMoved;
    evt.mouseMove.x = std::floor(m_current_pointer_position.x);
    evt.mouseMove.y = std::floor(m_current_pointer_position.y);
    m_new_events.push_back(evt);
}
void WindowImplWayland::handleWaylandPointerButton(uint32_t time,uint32_t button,uint32_t state) {
    Event evt;
    evt.type = (state == WL_POINTER_BUTTON_STATE_RELEASED) ? Event::MouseButtonReleased : Event::MouseButtonPressed;
    switch (button) {
    case BTN_LEFT:
        evt.mouseButton.button = Mouse::Button::Left;
        break;
    case BTN_RIGHT:
        evt.mouseButton.button = Mouse::Button::Right;
        break;
    case BTN_MIDDLE:
        evt.mouseButton.button = Mouse::Button::Middle;
        break;
    case BTN_SIDE://#4,so XButton1?
        evt.mouseButton.button = Mouse::Button::XButton1;
        break;
    case BTN_EXTRA://#5,so XButton1?
        evt.mouseButton.button = Mouse::Button::XButton2;
        break;
    default:
        err() << "Unidentified button " << button << std::endl;
        std::abort();
    }
    evt.mouseButton.x = std::floor(m_current_pointer_position.x);
    evt.mouseButton.y = std::floor(m_current_pointer_position.y);
    m_new_events.push_back(evt);
}

////////////////////////////////////////////////////////////
WindowImplWayland::WindowImplWayland(VideoMode mode, const String& title, unsigned long style, const ContextSettings& settings) :
//m_window         (0),
m_screen         (0),
//m_inputMethod    (NULL),
//m_inputContext   (NULL),
m_isExternal     (false),
m_oldVideoMode   (0),
//m_oldRRCrtc      (0),
//m_hiddenCursor   (0),
//m_lastCursor     (None),
m_keyRepeat      (true),
m_previousSize   (-1, -1),
m_useSizeHints   (false),
m_fullscreen     ((style & Style::Fullscreen) != 0),
m_cursorGrabbed  (m_fullscreen),
m_windowMapped   (false),
//m_iconPixmap     (0),
//m_iconMaskPixmap (0),
//m_lastInputTime  (0)
m_xdg_initial_configure_seen(false)
{
    m_window_size.x = mode.width;
    m_window_size.y = mode.height;

    m_current_pointer_position.x = 0.;
    m_current_pointer_position.y = 0.;

    // Open a connection with the X server
    m_display = OpenWaylandDisplay();

    if (m_display->the_window) {
        err() << "More than one window" << std::endl;
        std::abort();
    }
    m_display->the_window = this;

    m_surface = wl_compositor_create_surface(m_display->compositor);
    m_egl_window = wl_egl_window_create(m_surface, m_window_size.x, m_window_size.y);

    m_xdg_surface = xdg_wm_base_get_xdg_surface(m_display->xdg_wm_base, m_surface);
    m_xdg_toplevel = xdg_surface_get_toplevel(m_xdg_surface);
    xdg_surface_add_listener(m_xdg_surface, &window_xdg_surface_listener, this);
    xdg_toplevel_add_listener(m_xdg_toplevel, &window_xdg_toplevel_listener, this);

    m_xdg_toplevel_deco = zxdg_decoration_manager_v1_get_toplevel_decoration(m_display->decoration_manager, m_xdg_toplevel);
    zxdg_toplevel_decoration_v1_set_mode(m_xdg_toplevel_deco, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);

    xdg_toplevel_set_title(m_xdg_toplevel, (const char*)title.toUtf8().c_str());
    wl_surface_commit(m_surface);

    wl_display_flush(m_display->display);


    initialize();
}


////////////////////////////////////////////////////////////
WindowImplWayland::~WindowImplWayland()
{
    // Cleanup graphical resources
    cleanup();

    // Close the connection with the X server
    CloseWaylandDisplay(m_display);

    // Remove this window from the global list of windows (required for focus request)
    Lock lock(allWindowsMutex);
    allWindows.erase(std::find(allWindows.begin(), allWindows.end(), this));
}


////////////////////////////////////////////////////////////
WindowHandle WindowImplWayland::getSystemHandle() const
{
    // Thankfully, unsigned long fits a pointer
    return (unsigned long)(void*)m_egl_window;
}



////////////////////////////////////////////////////////////
void WindowImplWayland::processEvents()
{
    // Process events. The callbacks related to this window will call
    // implementation functions that in turn do stuff and append to the
    // m_new_events queue
    wl_display_dispatch_pending(m_display->display);

    Event e;
    while (!m_new_events.empty()) {
        e = m_new_events.front();
        m_new_events.pop_front();
        pushEvent(e);
    }

    // Process clipboard window events
    priv::ClipboardImplWayland::processEvents();
}


////////////////////////////////////////////////////////////
Vector2i WindowImplWayland::getPosition() const
{
    return Vector2i(0,0);
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setPosition(const Vector2i& position)
{
}


////////////////////////////////////////////////////////////
Vector2u WindowImplWayland::getSize() const
{
    return m_window_size;
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setSize(const Vector2u& size)
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setTitle(const String& title)
{
    xdg_toplevel_set_title(m_xdg_toplevel, (const char*)title.toUtf8().c_str());
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setIcon(unsigned int width, unsigned int height, const Uint8* pixels)
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setVisible(bool visible)
{
    err() << "called setVisible" << std::endl;
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setMouseCursorVisible(bool visible)
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setMouseCursor(const CursorImpl& cursor)
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setMouseCursorGrabbed(bool grabbed)
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setKeyRepeatEnabled(bool enabled)
{
    m_keyRepeat = enabled;
}


////////////////////////////////////////////////////////////
void WindowImplWayland::requestFocus()
{
}


////////////////////////////////////////////////////////////
bool WindowImplWayland::hasFocus() const
{
    return true;
}


////////////////////////////////////////////////////////////
void WindowImplWayland::grabFocus()
{
    err() << "called grabFocus" << std::endl;

}


////////////////////////////////////////////////////////////
void WindowImplWayland::setVideoMode(const VideoMode& mode)
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::resetVideoMode()
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::switchToFullscreen()
{
    grabFocus();
}


////////////////////////////////////////////////////////////
void WindowImplWayland::setProtocols()
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::initialize()
{
    // Deep magic, see xdg shell docs about waiting for configure
    while (!m_xdg_initial_configure_seen) {
        wl_display_flush(m_display->display);
        wl_display_dispatch(m_display->display);
    }
}

////////////////////////////////////////////////////////////
void WindowImplWayland::createHiddenCursor()
{
}


////////////////////////////////////////////////////////////
void WindowImplWayland::cleanup()
{
    // Restore the previous video mode (in case we were running in fullscreen)
    resetVideoMode();

    // Unhide the mouse cursor (in case it was hidden)
    setMouseCursorVisible(true);
}

} // namespace priv

} // namespace sf
