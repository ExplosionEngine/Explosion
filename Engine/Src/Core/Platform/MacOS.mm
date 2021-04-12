#include <Explosion/Core/Platform.h>

#ifdef TARGET_OS_MAC
const char* INSTANCE_EXTENSIONS[] = {
    VK_MVK_MACOS_SURFACE_EXTENSION_NAME
};

uint32_t GetPlatformInstanceExtensionNum()
{
    return sizeof(INSTANCE_EXTENSIONS) / sizeof(const char*);
}

const char** GetPlatformInstanceExtensions()
{
    return INSTANCE_EXTENSIONS;
}
#endif
