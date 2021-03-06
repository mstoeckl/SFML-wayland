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
#include <SFML/Window/Unix/Wayland/VideoModeImplWayland.hpp>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <SFML/System/Err.hpp>
#include <algorithm>


namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
std::vector<VideoMode> VideoModeImplWayland::getFullscreenModes()
{
    std::vector<VideoMode> modes;

    // TODO: use xdg_output to get display size!
    modes.push_back(VideoMode(1024, 768));
    return modes;
}


////////////////////////////////////////////////////////////
VideoMode VideoModeImplWayland::getDesktopMode()
{
    VideoMode desktopMode;
    WaylandDisplay* display = OpenWaylandDisplay();
    if (display->output_width > 0 && display->output_height > 0) {
        // TODO: actually check this. Also, what if there are multiple outputs?
        desktopMode = VideoMode(display->output_width / display->output_scale,
                                display->output_height / display->output_scale);
    } else {
        // Fallback, in case mode not provided
        desktopMode = VideoMode(800, 600);
    }

    CloseWaylandDisplay(display);
    return desktopMode;
}

} // namespace priv

} // namespace sf
