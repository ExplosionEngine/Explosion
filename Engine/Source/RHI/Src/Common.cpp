//
// Created by johnk on 2023/3/25.
//

#include <RHI/Common.h>

#define RHI_FLAGS_IMPL(FlagsType, BitsType) \
    FlagsType operator&(BitsType a, BitsType b) { return FlagsType(static_cast<FlagsType::UnderlyingType>(a) & static_cast<FlagsType::UnderlyingType>(b)); } \
    FlagsType operator&(FlagsType a, BitsType b) { return FlagsType(a.Value() & static_cast<FlagsType::UnderlyingType>(b)); } \
    FlagsType operator|(BitsType a, BitsType b) { return FlagsType(static_cast<FlagsType::UnderlyingType>(a) | static_cast<FlagsType::UnderlyingType>(b)); } \
    FlagsType operator|(FlagsType a, BitsType b) { return FlagsType(a.Value() | static_cast<FlagsType::UnderlyingType>(b)); } \

namespace RHI {
    RHI_FLAGS_IMPL(BufferUsageFlags, BufferUsageBits)
    RHI_FLAGS_IMPL(TextureUsageFlags, TextureUsageBits)
    RHI_FLAGS_IMPL(ShaderStageFlags, ShaderStageBits)
    RHI_FLAGS_IMPL(ColorWriteFlags, ColorWriteBits)
}

namespace RHI {
    size_t GetBytesPerPixel(PixelFormat format)
    {
        if (format > PixelFormat::begin8Bits && format < PixelFormat::begin16Bits) {
            return 1;
        }
        if (format > PixelFormat::begin16Bits && format < PixelFormat::begin32Bits) {
            return 2;
        }
        if (format > PixelFormat::begin32Bits && format < PixelFormat::begin64Bits) {
            return 4;
        }
        if (format > PixelFormat::begin64Bits && format < PixelFormat::begin128Bits) {
            return 8;
        }
        if (format > PixelFormat::begin128Bits && format < PixelFormat::max) {
            return 16;
        }
        return Assert(false), 1;
    }
}
