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
#include <SFML/Window/Unix/Wayland/VulkanImplWayland.hpp>
#include <SFML/Window/Unix/Wayland/DisplayWayland.hpp>
#include <dlfcn.h>
#define VK_USE_PLATFORM_XLIB_KHR
#define VK_NO_PROTOTYPES
#include <vulkan.h>
#include <string>
#include <map>
#include <cstring>

namespace sf
{
namespace priv
{
////////////////////////////////////////////////////////////
bool VulkanImplWayland::isAvailable(bool requireGraphics)
{
    return false;
}


////////////////////////////////////////////////////////////
VulkanFunctionPointer VulkanImplWayland::getFunction(const char* name)
{
    return 0;
}


////////////////////////////////////////////////////////////
const std::vector<const char*>& VulkanImplWayland::getGraphicsRequiredInstanceExtensions()
{
    static std::vector<const char*> extensions;
    return extensions;
}


////////////////////////////////////////////////////////////
bool VulkanImplWayland::createVulkanSurface(const VkInstance& instance, WindowHandle windowHandle, VkSurfaceKHR& surface, const VkAllocationCallbacks* allocator)
{
    return false;
}

} // namespace priv

} // namespace sf
