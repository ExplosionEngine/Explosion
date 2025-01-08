//
// Created by johnk on 2025/1/8.
//

#pragma once

namespace Render {
    class Surface {
    public:
        virtual ~Surface();

    protected:
        Surface();
    };

    class SwapChainSurface final : public Surface {
    public:
        explicit SwapChainSurface(void* inNativeWindow);
        ~SwapChainSurface() override;
    };

    class TextureSurface final : public Surface {
    public:
        explicit TextureSurface(RHI::Texture* inRhiTexture);
        ~TextureSurface() override;
    };
}
