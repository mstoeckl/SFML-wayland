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

#ifndef SFML_WINDOWIMPLWAYLAND_HPP
#define SFML_WINDOWIMPLWAYLAND_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <SFML/Window/Event.hpp>
#include <SFML/Window/WindowImpl.hpp>
#include <SFML/System/String.hpp>
#include <SFML/Window/WindowStyle.hpp> // Prevent conflict with macro None from Xlib
#include <deque>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Linux (X11) implementation of WindowImpl
///
////////////////////////////////////////////////////////////
class WindowImplWayland : public WindowImpl
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Construct the window implementation from an existing control
    ///
    /// \param handle Platform-specific handle of the control
    ///
    ////////////////////////////////////////////////////////////
    WindowImplWayland(WindowHandle handle);

    ////////////////////////////////////////////////////////////
    /// \brief Create the window implementation
    ///
    /// \param mode  Video mode to use
    /// \param title Title of the window
    /// \param style Window style (resizable, fixed, or fullscren)
    /// \param settings Additional settings for the underlying OpenGL context
    ///
    ////////////////////////////////////////////////////////////
    WindowImplWayland(VideoMode mode, const String& title, unsigned long style, const ContextSettings& settings);

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~WindowImplWayland();

    ////////////////////////////////////////////////////////////
    /// \brief Get the OS-specific handle of the window
    ///
    /// \return Handle of the window
    ///
    ////////////////////////////////////////////////////////////
    virtual WindowHandle getSystemHandle() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the position of the window
    ///
    /// \return Position of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual Vector2i getPosition() const;

    ////////////////////////////////////////////////////////////
    /// \brief Change the position of the window on screen
    ///
    /// \param position New position of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual void setPosition(const Vector2i& position);

    ////////////////////////////////////////////////////////////
    /// \brief Get the client size of the window
    ///
    /// \return Size of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual Vector2u getSize() const;

    ////////////////////////////////////////////////////////////
    /// \brief Change the size of the rendering region of the window
    ///
    /// \param size New size, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual void setSize(const Vector2u& size);

    ////////////////////////////////////////////////////////////
    /// \brief Change the title of the window
    ///
    /// \param title New title
    ///
    ////////////////////////////////////////////////////////////
    virtual void setTitle(const String& title);

    ////////////////////////////////////////////////////////////
    /// \brief Change the window's icon
    ///
    /// \param width  Icon's width, in pixels
    /// \param height Icon's height, in pixels
    /// \param pixels Pointer to the pixels in memory, format must be RGBA 32 bits
    ///
    ////////////////////////////////////////////////////////////
    virtual void setIcon(unsigned int width, unsigned int height, const Uint8* pixels);

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the window
    ///
    /// \param visible True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    virtual void setVisible(bool visible);

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the mouse cursor
    ///
    /// \param visible True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    virtual void setMouseCursorVisible(bool visible);

    ////////////////////////////////////////////////////////////
    /// \brief Grab or release the mouse cursor
    ///
    /// \param grabbed True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    virtual void setMouseCursorGrabbed(bool grabbed);

    ////////////////////////////////////////////////////////////
    /// \brief Set the displayed cursor to a native system cursor
    ///
    /// \param cursor Native system cursor type to display
    ///
    ////////////////////////////////////////////////////////////
    virtual void setMouseCursor(const CursorImpl& cursor);

    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable automatic key-repeat
    ///
    /// \param enabled True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    virtual void setKeyRepeatEnabled(bool enabled);

    ////////////////////////////////////////////////////////////
    /// \brief Request the current window to be made the active
    ///        foreground window
    ///
    ////////////////////////////////////////////////////////////
    virtual void requestFocus();

    ////////////////////////////////////////////////////////////
    /// \brief Check whether the window has the input focus
    ///
    /// \return True if window has focus, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    virtual bool hasFocus() const;



    void handleWaylandSurfaceConfigure(struct xdg_surface *xdg_surface,
                      uint32_t serial);
    void handleWaylandToplevelClose(struct xdg_toplevel *xdg_toplevel);
    void handleWaylandToplevelConfigure(
           struct xdg_toplevel *xdg_toplevel,
           int32_t width,
           int32_t height,
             struct wl_array *states);
    void handleWaylandKeyboardText(uint32_t letter);
    void handleWaylandPointerButton(uint32_t time,uint32_t button,uint32_t state);
    void handleWaylandPointerMotion(uint32_t time,double x, double y);
    void handleWaylandKeyboardKey(uint32_t time,uint32_t key,uint32_t state,bool shift, bool control, bool system, bool alt);
    void handleWaylandPointerAxis(uint32_t time,uint32_t axis, double val);
    void handleWaylandPointerAxisDiscrete(uint32_t axis, int val);
    void handleWaylandPointerEnter();
    void handleWaylandPointerLeave();
    void handleWaylandKeyboardEnter();
    void handleWaylandKeyboardLeave();

    ////////////////////////////////////////////////////////////
    /// \brief Process incoming events from the operating system
    ///
    ////////////////////////////////////////////////////////////
    virtual void processEvents();

private:
    ////////////////////////////////////////////////////////////
    /// \brief Do some common initializations after the window has been created
    ///
    ////////////////////////////////////////////////////////////
    void initialize();

    ////////////////////////////////////////////////////////////
    /// \brief Cleanup graphical resources attached to the window
    ///
    ////////////////////////////////////////////////////////////
    void cleanup();

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
public:
    WaylandDisplay*         m_display;        ///< Pointer to the display
    struct wl_surface*      m_surface;
    struct xdg_surface*     m_xdg_surface;
    struct xdg_toplevel*    m_xdg_toplevel;
    struct zxdg_toplevel_decoration_v1* m_xdg_toplevel_deco;
    struct wl_egl_window*   m_egl_window;

    bool m_xdg_initial_configure_seen;
private:
    int                m_screen;         ///< Screen identifier

    std::deque<Event> m_new_events;
    Vector2f          m_current_pointer_position; // as of new_event queue

    Vector2u m_window_size;

    bool               m_isExternal;     ///< Tell whether the window has been created externally or by SFML
    int                m_oldVideoMode;   ///< Video mode in use before we switch to fullscreen
    bool               m_keyRepeat;      ///< Is the KeyRepeat feature enabled?
    Vector2i           m_previousSize;   ///< Previous size of the window, to find if a ConfigureNotify event is a resize event (could be a move event only)
    bool               m_useSizeHints;   ///< Is the size of the window fixed with size hints?
    bool               m_fullscreen;     ///< Is the window in fullscreen?
    bool               m_cursorGrabbed;  ///< Is the mouse cursor trapped?
    bool               m_windowMapped;   ///< Has the window been mapped by the window manager?
};

} // namespace priv

} // namespace sf


#endif // SFML_WINDOWIMPLWAYLAND_HPP
