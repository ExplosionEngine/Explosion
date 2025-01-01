#pragma once

#ifndef PLATFORM_H
#define PLATFORM_H

/// \file Platform.hpp
/// \brief Macros to handle Vulkan-specific attributes in shader code.

/// If VULKAN is defined, these macros expand to Vulkan binding/attribute specifiers.
/// Otherwise, they expand to nothing.

// If building with Vulkan, define the macros with valid attributes
#if defined(VULKAN)
    #define VK_BINDING(set_index, binding_index) [[vk::binding(set_index, binding_index)]]
    #define VK_LOCATION(location_index)          [[vk::location(location_index)]]
#else
    #define VK_BINDING(set_index, binding_index)
    #define VK_LOCATION(location_index)
#endif

#endif // PLATFORM_H
