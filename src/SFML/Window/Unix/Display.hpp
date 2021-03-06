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

#ifndef SFML_SHAREDDISPLAY_HPP
#define SFML_SHAREDDISPLAY_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <string>

namespace sf
{
namespace priv
{
enum DisplayType {
    Wayland,
    X11
};
////////////////////////////////////////////////////////////
/// \brief Try to connect to a Wayland/X11 display, and report which works
///
/// This never returns Unknown.
////////////////////////////////////////////////////////////
enum DisplayType getDisplayType();

////////////////////////////////////////////////////////////
/// \brief Close the connection made by getDisplayType
///
/// Call after getDisplayType has been used to pick a display, and after
/// whatever class based on the display type has been initialized. (This
/// ensures we don't "double-connnect".)
////////////////////////////////////////////////////////////
void unrefDisplay();

} // namespace priv

} // namespace sf


#endif // SFML_SHAREDDISPLAY_HPP


