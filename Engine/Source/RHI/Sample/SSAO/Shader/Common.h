#ifndef __COMMON_H__
#define __COMMON_H__

#if VULKAN
#define VK_BINDING(x, y) [[vk::binding(x, y)]]
#define VK_LOCATION(x)   [[vk::location(x)]]
#else
#define VK_BINDING(x, y)
#define VK_LOCATION(x)
#endif

#endif  // !__COMMON_H__