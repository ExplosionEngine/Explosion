//
// Created by Zach Lee on 2022/10/1.
//

#pragma once

#import <Metal/Metal.h>
#include <RHI/TextureView.h>

namespace RHI::Metal {
    class MTLDevice;

    class MTLTextureView : public TextureView {
    public:
        MTLTextureView(MTLDevice &device, const TextureViewCreateInfo* createInfo);
        ~MTLTextureView();

        void Destroy() override;
    private:

    };
}