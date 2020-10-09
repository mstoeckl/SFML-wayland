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
#include <SFML/Window/Unix/X11/VulkanImplX11.hpp>
#include <SFML/Window/Unix/Wayland/VulkanImplWayland.hpp>
#include <SFML/Window/Unix/VulkanImpl.hpp>
#include <SFML/Window/Unix/Display.hpp>

namespace sf
{
namespace priv
{
bool VulkanImpl::isAvailable(bool requireGraphics) {
    bool r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = VulkanImplWayland::isAvailable(requireGraphics);
    } else {
        r = VulkanImplX11::isAvailable(requireGraphics);
    }
    unrefDisplay();
    return r;
}

VulkanFunctionPointer VulkanImpl::getFunction(const char* name) {
    VulkanFunctionPointer r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = VulkanImplWayland::getFunction(name);
    } else {
        r = VulkanImplX11::getFunction(name);
    }
    unrefDisplay();
    return r;
}

const std::vector<const char*>& VulkanImpl::getGraphicsRequiredInstanceExtensions() {
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        const std::vector<const char*>& r = VulkanImplWayland::getGraphicsRequiredInstanceExtensions();
        unrefDisplay();
        return r;
    } else {
        const std::vector<const char*>& r = VulkanImplX11::getGraphicsRequiredInstanceExtensions();
        unrefDisplay();
        return r;
    }
}

bool VulkanImpl::createVulkanSurface(const VkInstance& instance, WindowHandle windowHandle, VkSurfaceKHR& surface, const VkAllocationCallbacks* allocator) {
    bool r;
    DisplayType displayType = getDisplayType();
    if (displayType == Wayland) {
        r = VulkanImplWayland::createVulkanSurface(instance, windowHandle, surface, allocator);
    } else {
        r = VulkanImplX11::createVulkanSurface(instance, windowHandle, surface, allocator);
    }
    unrefDisplay();
    return r;
}

}

} // namespace sf
