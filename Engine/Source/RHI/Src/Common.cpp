//
// Created by johnk on 2023/3/25.
//

#include <RHI/Common.h>

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
