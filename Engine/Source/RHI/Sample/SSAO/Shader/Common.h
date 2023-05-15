#ifndef __COMMON_H__
#define __COMMON_H__

#if VULKAN
#define VK_BINDING(x, y) [[vk::binding(x, y)]]
#else
#define VK_BINDING(x, y)
#endif

#endif  // !__COMMON_H__