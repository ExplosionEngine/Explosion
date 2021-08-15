//
// Created by John Kindem on 2021/5/26.
//

#ifndef EXPLOSION_DESCRIPTORSET_H
#define EXPLOSION_DESCRIPTORSET_H

#include <vector>

#include <RHI/Enum.h>

namespace Explosion::RHI {
    class DescriptorPool;
    class GraphicsPipeline;
    class Buffer;
    class Sampler;
    class ImageView;

    class DescriptorSet {
    public:
        struct DescriptorBufferInfo {
            Buffer* buffer = nullptr;
            uint32_t offset = 0;
            uint32_t range = 0;
        };

        struct DescriptorTextureInfo {
            Sampler* sampler = nullptr;
            ImageView* imageView = nullptr;
            ImageLayout imageLayout = ImageLayout::UNDEFINED;
        };

        struct DescriptorWriteInfo {
            uint32_t binding = 0;
            DescriptorType type = DescriptorType::MAX;
            DescriptorBufferInfo* bufferInfo = nullptr;
            DescriptorTextureInfo* textureInfo = nullptr;
        };

        virtual ~DescriptorSet();
        virtual void WriteDescriptors(const std::vector<DescriptorWriteInfo>& writeInfos) = 0;

    protected:
        DescriptorSet();
    };
}

#endif //EXPLOSION_DESCRIPTORSET_H
