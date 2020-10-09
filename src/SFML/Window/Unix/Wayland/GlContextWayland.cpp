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
//#define SF_GLAD_GLX_IMPLEMENTATION
#include <SFML/Window/Unix/Wayland/WindowImplWayland.hpp> // important to be included first (conflict with None)
#include <SFML/Window/Unix/Wayland/GlContextWayland.hpp>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <SFML/System/Mutex.hpp>
#include <SFML/System/Lock.hpp>
#include <SFML/System/Err.hpp>
#include <EGL/egl.h>
//#include <SFML/Window/EGLCheck.hpp>
#include <vector>

#if !defined(GLX_DEBUGGING) && defined(SFML_DEBUG)
    // Enable this to print messages to err() everytime GLX produces errors
    //#define GLX_DEBUGGING
#endif

namespace
{
    sf::Mutex glxErrorMutex;
    bool glxErrorOccurred = false;


    EGLConfig chooseConfig (const sf::ContextSettings& settings, EGLDisplay egl_dpy, bool for_window ) {
        // Create EGL context
        EGLint num_configs = -1;
        if (eglGetConfigs(egl_dpy, NULL, 0, &num_configs) == EGL_FALSE) {
            sf::err() << "Failed to count configs" << std::endl;
            std::abort();
        }

        EGLint conf_attr_list[11] = {
            // cannot do both window&pbuffer, it seems
            EGL_SURFACE_TYPE, for_window ? EGL_WINDOW_BIT : EGL_PBUFFER_BIT,
    //        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES3_BIT,
            EGL_BLUE_SIZE, 1,
           EGL_GREEN_SIZE, 1,
           EGL_RED_SIZE, 1,
            EGL_DEPTH_SIZE, std::min((EGLint)settings.depthBits, 16),
            EGL_NONE};
        EGLint num_matches = -1;
        std::vector<EGLConfig> configs(num_configs);
        if (eglChooseConfig(egl_dpy,conf_attr_list, configs.data(),num_configs,&num_matches) == EGL_FALSE) {
            sf::err() << "Failed to get list of configs" << std::endl;
            std::abort();
        }

        if (num_matches <= 0) {
            sf::err() << "No matching configs" << std::endl;
            std::abort();
        }
        configs.resize(num_matches);
        sf::err() << configs.size() << " matching configs" << std::endl;

        for (EGLConfig c : configs) {
            // TODO: use eglGetConfigAttrib to figure out all the properties

            EGLint val;
            EGLBoolean v = eglGetConfigAttrib(egl_dpy, c, EGL_DEPTH_SIZE, &val);
//            sf::err() << val << " " << v << " " <<  c << std::endl;
        }
        return configs[0];
    }
}


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
GlContextWayland::GlContextWayland(GlContextWayland* shared) :
m_display   (NULL),
m_surface(EGL_NO_SURFACE),
m_context   (EGL_NO_CONTEXT),
m_ctx_config   (NULL),
m_ownsWindow(false)
{
    // Save the creation settings
    m_settings = ContextSettings();

    // Open the connection with the X server
    m_display = OpenWaylandDisplay();

    err() << "Making a GlxContext context from nothing" << std::endl;
    err() << "SKIPPING" << std::endl;
    return;
//    return; // cannot make pbuffer, it seems. Or can we?


    // Make sure that extensions are initialized
//    ensureExtensionsInit(m_display, DefaultScreen(m_display));

    m_ctx_config = chooseConfig(m_settings, m_display->egl_dpy, false);

    // Create the context (and pick config, first)
    createContext(shared);

    // Create the rendering surface (window or pbuffer if supported)
    createSurface(shared, 1, 1, VideoMode::getDesktopMode().bitsPerPixel);

}


////////////////////////////////////////////////////////////
GlContextWayland::GlContextWayland(GlContextWayland* shared, const ContextSettings& settings, const WindowImpl* owner, unsigned int bitsPerPixel) :
m_display   (NULL),
m_surface (EGL_NO_SURFACE),
m_context   (EGL_NO_CONTEXT),
m_ctx_config   (NULL),
m_ownsWindow(false)
{
    // Save the creation settings
    m_settings = settings;

    // Open the connection with the X server
    m_display = OpenWaylandDisplay();


    err() << "Calling GlxContext constructor from a window" << std::endl;

    m_ctx_config = chooseConfig(m_settings, m_display->egl_dpy, true);

    // Create context before creating window surface
    createContext(shared);

    err() << m_ctx_config << " " << m_context << std::endl;

    WindowImplWayland* window = (WindowImplWayland*)owner;
    createSurface(window->m_egl_window);
//   makeCurrent(true);

//   eglMakeCurrent(window->m_display, window->m_display)



//    createSurface(window);


    // Make sure that extensions are initialized
//    ensureExtensionsInit(m_display, DefaultScreen(m_display));

    // Create the rendering surface from the owner window
//    createSurface(static_cast< ::Window>(owner->getSystemHandle()));

    // Create the context
}


////////////////////////////////////////////////////////////
GlContextWayland::GlContextWayland(GlContextWayland* shared, const ContextSettings& settings, unsigned int width, unsigned int height) :
m_display   (NULL),
m_surface (EGL_NO_SURFACE),
m_context   (EGL_NO_CONTEXT),
m_ctx_config   (NULL),
m_ownsWindow(false)
{
    // Save the creation settings
    m_settings = settings;

    // Open the connection with the X server
    m_display = OpenWaylandDisplay();

    err() << "Calling GlxContext constructor from width/height pair" << std::endl;

    // Make sure that extensions are initialized
//    ensureExtensionsInit(m_display, DefaultScreen(m_display));

    m_ctx_config = chooseConfig(m_settings, m_display->egl_dpy, false);

    // Create the context
    createContext(shared);

    // Create the rendering surface (window or pbuffer if supported)
    createSurface(shared, width, height, VideoMode::getDesktopMode().bitsPerPixel);

}


////////////////////////////////////////////////////////////
GlContextWayland::~GlContextWayland()
{
    // Notify unshared OpenGL resources of context destruction
    cleanupUnsharedResources();


    // Close the connection with the X server
    CloseWaylandDisplay(m_display);
}


////////////////////////////////////////////////////////////
GlFunctionPointer GlContextWayland::getFunction(const char* name)
{

    return reinterpret_cast<GlFunctionPointer>(eglGetProcAddress(name));
}


////////////////////////////////////////////////////////////
bool GlContextWayland::makeCurrent(bool current)
{
    err() << "Calling makeCurrent with current=" << current  << std::endl;
    if (!m_context)
        return false;

#if defined(GLX_DEBUGGING)
    GlxErrorHandler handler(m_display);
#endif

    bool result = false;

    if (current) {
        result = eglMakeCurrent(m_display->egl_dpy,m_surface, m_surface, m_context );
        // TODO
    } else {
        result = eglMakeCurrent(m_display->egl_dpy,  EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
    }

#if defined(GLX_DEBUGGING)
    if (glxErrorOccurred)
        err() << "GLX error in GlxContext::makeCurrent()" << std::endl;
#endif

    return result;
}


////////////////////////////////////////////////////////////
void GlContextWayland::display()
{
#if defined(GLX_DEBUGGING)
    GlxErrorHandler handler(m_display);
#endif

    if (m_surface) {
        if (eglSwapBuffers(m_display->egl_dpy, m_surface) == EGL_FALSE) {
            err() << "Swap buffers failed" << std::endl;
            std::abort();
        }
    }

#if defined(GLX_DEBUGGING)
    if (glxErrorOccurred)
        err() << "GLX error in GlxContext::display()" << std::endl;
#endif
}


////////////////////////////////////////////////////////////
void GlContextWayland::setVerticalSyncEnabled(bool enabled)
{
    int result = 0;

    if (result != 0)
        err() << "Setting vertical sync failed" << std::endl;
}



//////////////////////////////////////////////////////////////
void GlContextWayland::createSurface(GlContextWayland* shared, unsigned int width, unsigned int height, unsigned int bitsPerPixel)
{

    EGLint attrib_list[9] = {
        EGL_WIDTH, (EGLint)width,
        EGL_HEIGHT, (EGLint)height,
        EGL_NONE
    };

    m_surface = eglCreatePbufferSurface(m_display->egl_dpy, m_ctx_config, attrib_list);
    if (m_surface == EGL_FALSE) {
        err() << "Failed to create pixel buffer surface: " << std::hex << eglGetError() << std::endl;
        std::abort();
    }
}


//////////////////////////////////////////////////////////////
void GlContextWayland::createSurface(struct wl_egl_window* window)
{
    // A window already exists, so just use it
    err() << "Calling createSurface(window)" << std::endl;

    EGLAttrib attrib_list[1] = {EGL_NONE};
    m_surface = eglCreatePlatformWindowSurface(m_display->egl_dpy,
                                              m_ctx_config,
                                              window,
                                              attrib_list);
   if (m_surface == EGL_NO_SURFACE) {
       err() << "Failed to create surface for window: " << std::hex << eglGetError() << std::endl;
        std::abort();
   }

//    m_window = window;

//    updateSettingsFromWindow();
}


////////////////////////////////////////////////////////////
void GlContextWayland::createContext(GlContextWayland* shared)
{
    EGLint attrlist[5]= {
//        EGL_CONTEXT_CLIENT_VERSION,3,
        EGL_NONE};
    m_context = eglCreateContext(m_display->egl_dpy, m_ctx_config,
            shared ? shared->m_context : EGL_NO_CONTEXT, attrlist);
    if (m_context == EGL_NO_CONTEXT) {
        err() << "Failed to make an EGL context" << std::endl;
        std::abort();
    }
}

} // namespace priv

} // namespace sf
