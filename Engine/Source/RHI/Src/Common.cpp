//
// Created by johnk on 2023/3/25.
//

#include <RHI/Common.h>

namespace RHI {
    size_t GetBytesPerPixel(PixelFormat format)
    {
        struct BytesPerPixelRange {
            PixelFormat begin;
            PixelFormat end;
            size_t bytesPerPixel;
        };
        static constexpr BytesPerPixelRange ranges[] = {
            { PixelFormat::begin8Bits,   PixelFormat::begin16Bits,  1 },
            { PixelFormat::begin16Bits,  PixelFormat::begin32Bits,  2 },
            { PixelFormat::begin32Bits,  PixelFormat::begin64Bits,  4 },
            { PixelFormat::begin64Bits,  PixelFormat::begin128Bits, 8 },
            { PixelFormat::begin128Bits, PixelFormat::max,          16 },
        };

        for (const auto& range : ranges) {
            if (format > range.begin && format < range.end) {
                return range.bytesPerPixel;
            }
        }
        return Assert(false), 1;
    }
}
