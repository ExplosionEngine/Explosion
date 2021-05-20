//
// Created by John Kindem on 2021/4/24.
//

#include <Explosion/RHI/Vulkan/VulkanAdapater.h>

namespace Explosion::RHI {
#define FIND_OR_EXCEPT                                            \
    auto iter = MAP.find(value);                                  \
    if (iter == MAP.end()) {                                      \
        throw std::runtime_error("found no suitable key in map"); \
    }                                                             \
    return iter->second;                                          \

#define VK_CONVERT_INSTANCE_B(Type, VkType)           \
    template <>                                       \
    VkType VkConvert<Type, VkType>(const Type& value) \
    {                                                 \

#define VK_CONVERT_INSTANCE_E \
    }                         \

#define GET_ENUM_BY_VK_INSTANCE_B(VkType, Type)         \
    template <>                                         \
    Type GetEnumByVk<VkType, Type>(const VkType& value) \
    {                                                   \

#define GET_ENUM_BY_VK_INSTANCE_E \
    }                             \

#define VK_GET_FLAGS_INSTANCE_B(Type, VkType)     \
    template <>                                   \
    VkFlags VkGetFlags<Type, VkType>(Flags flags) \
    {                                             \

#define VK_GET_FLAGS_INSTANCE_E \
    }                           \

#define COMBINE_FLAGS_AND_RETURN             \
    VkFlags result = 0;                      \
    for (auto& pair : MAP) {                 \
        if (flags & FlagsCast(pair.first)) { \
            result |= pair.second;           \
        }                                    \
    }                                        \
    return result;                           \

}

namespace Explosion::RHI {
    VkBool32 VkBoolConvert(bool value)
    {
        return value ? VK_TRUE : VK_FALSE;
    }

    VK_CONVERT_INSTANCE_B(Format, VkFormat)
        static std::unordered_map<Format, VkFormat> MAP = {
            { Format::UNDEFINED, VkFormat::VK_FORMAT_UNDEFINED },
            { Format::R8_G8_B8_A8_RGB, VkFormat::VK_FORMAT_R8G8B8A8_SRGB },
            { Format::B8_G8_R8_A8_RGB, VkFormat::VK_FORMAT_B8G8R8A8_SRGB },
            { Format::R32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32_UINT },
            { Format::R32_SIGNED_INT, VkFormat::VK_FORMAT_R32_SINT },
            { Format::R32_FLOAT, VkFormat::VK_FORMAT_R32_SFLOAT },
            { Format::R32_G32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32G32_UINT },
            { Format::R32_G32_SIGNED_INT, VkFormat::VK_FORMAT_R32G32_SINT },
            { Format::R32_G32_FLOAT, VkFormat::VK_FORMAT_R32G32_SFLOAT },
            { Format::R32_G32_B32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32G32B32_UINT },
            { Format::R32_G32_B32_SIGNED_INT, VkFormat::VK_FORMAT_R32G32B32_SINT },
            { Format::R32_G32_B32_FLOAT, VkFormat::VK_FORMAT_R32G32B32_SFLOAT },
            { Format::R32_G32_B32_A32_UNSIGNED_INT, VkFormat::VK_FORMAT_R32G32B32A32_UINT },
            { Format::R32_G32_B32_A32_SIGNED_INT, VkFormat::VK_FORMAT_R32G32B32A32_SINT },
            { Format::R32_G32_B32_A32_FLOAT, VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT },
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    GET_ENUM_BY_VK_INSTANCE_B(VkFormat, Format)
        static std::unordered_map<VkFormat, Format> MAP = {
            { VkFormat::VK_FORMAT_UNDEFINED, Format::UNDEFINED },
            { VkFormat::VK_FORMAT_R8G8B8A8_SRGB, Format::R8_G8_B8_A8_RGB },
            { VkFormat::VK_FORMAT_B8G8R8A8_SRGB, Format::B8_G8_R8_A8_RGB },
            { VkFormat::VK_FORMAT_R32_UINT, Format::R32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32_SINT, Format::R32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32_SFLOAT, Format::R32_FLOAT },
            { VkFormat::VK_FORMAT_R32G32_UINT, Format::R32_G32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32_SINT, Format::R32_G32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32_SFLOAT, Format::R32_G32_FLOAT },
            { VkFormat::VK_FORMAT_R32G32B32_UINT, Format::R32_G32_B32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32_SINT, Format::R32_G32_B32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32_SFLOAT, Format::R32_G32_B32_FLOAT },
            { VkFormat::VK_FORMAT_R32G32B32A32_UINT, Format::R32_G32_B32_A32_UNSIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32A32_SINT, Format::R32_G32_B32_A32_SIGNED_INT },
            { VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT, Format::R32_G32_B32_A32_FLOAT },
        };
        FIND_OR_EXCEPT
    GET_ENUM_BY_VK_INSTANCE_E

    VK_CONVERT_INSTANCE_B(ImageType, VkImageType)
        static std::unordered_map<ImageType, VkImageType> MAP = {
            { ImageType::IMAGE_1D, VkImageType::VK_IMAGE_TYPE_1D },
            { ImageType::IMAGE_2D, VkImageType::VK_IMAGE_TYPE_2D },
            { ImageType::IMAGE_3D, VkImageType::VK_IMAGE_TYPE_3D }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(ImageViewType, VkImageViewType)
        static std::unordered_map<ImageViewType, VkImageViewType> MAP = {
            { ImageViewType::VIEW_1D, VkImageViewType::VK_IMAGE_VIEW_TYPE_1D },
            { ImageViewType::VIEW_2D, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D },
            { ImageViewType::VIEW_3D, VkImageViewType::VK_IMAGE_VIEW_TYPE_3D },
            { ImageViewType::VIEW_CUBE, VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE },
            { ImageViewType::VIEW_1D_ARRAY, VkImageViewType::VK_IMAGE_VIEW_TYPE_1D_ARRAY },
            { ImageViewType::VIEW_2D_ARRAY, VkImageViewType::VK_IMAGE_VIEW_TYPE_2D_ARRAY },
            { ImageViewType::VIEW_CUBE_ARRAY, VkImageViewType::VK_IMAGE_VIEW_TYPE_CUBE_ARRAY }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(AttachmentLoadOp, VkAttachmentLoadOp)
        static std::unordered_map<AttachmentLoadOp, VkAttachmentLoadOp> MAP = {
            { AttachmentLoadOp::NONE, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_DONT_CARE },
            { AttachmentLoadOp::CLEAR, VkAttachmentLoadOp::VK_ATTACHMENT_LOAD_OP_CLEAR }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(AttachmentStoreOp, VkAttachmentStoreOp)
        static std::unordered_map<AttachmentStoreOp, VkAttachmentStoreOp> MAP = {
            { AttachmentStoreOp::NONE, VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_DONT_CARE },
            { AttachmentStoreOp::STORE, VkAttachmentStoreOp::VK_ATTACHMENT_STORE_OP_STORE }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(VertexInputRate, VkVertexInputRate)
        static std::unordered_map<VertexInputRate, VkVertexInputRate> MAP = {
            { VertexInputRate::PER_VERTEX, VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX },
            { VertexInputRate::PER_INSTANCE, VkVertexInputRate::VK_VERTEX_INPUT_RATE_INSTANCE }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(FrontFace, VkFrontFace)
        static std::unordered_map<FrontFace, VkFrontFace> MAP = {
            { FrontFace::COUNTER_CLOCK_WISE, VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE },
            { FrontFace::CLOCK_WISE, VkFrontFace::VK_FRONT_FACE_CLOCKWISE },
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(DescriptorType, VkDescriptorType)
        static std::unordered_map<DescriptorType, VkDescriptorType> MAP = {
            { DescriptorType::UNIFORM_BUFFER, VkDescriptorType::VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER },
            { DescriptorType::STORAGE_BUFFER, VkDescriptorType::VK_DESCRIPTOR_TYPE_STORAGE_BUFFER },
            { DescriptorType::IMAGE_SAMPLER, VkDescriptorType::VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(BufferUsageBits, VkBufferUsageFlagBits)
        static std::unordered_map<BufferUsageBits, VkBufferUsageFlagBits> MAP = {
            {BufferUsageBits::VERTEX_BUFFER,  VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
            {BufferUsageBits::INDEX_BUFFER,   VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
            {BufferUsageBits::UNIFORM_BUFFER, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
            {BufferUsageBits::STORAGE_BUFFER, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT },
            {BufferUsageBits::TRANSFER_SRC,   VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
            {BufferUsageBits::TRANSFER_DST,   VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    VK_CONVERT_INSTANCE_B(ImageLayout, VkImageLayout)
        static std::unordered_map<ImageLayout, VkImageLayout> MAP = {
            {ImageLayout::UNDEFINED,                        VkImageLayout::VK_IMAGE_LAYOUT_UNDEFINED },
            {ImageLayout::COLOR_ATTACHMENT_OPTIMAL,         VkImageLayout::VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL },
            {ImageLayout::DEPTH_STENCIL_ATTACHMENT_OPTIMAL, VkImageLayout::VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL },
            {ImageLayout::TRANSFER_SRC_OPTIMAL,             VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL },
            {ImageLayout::TRANSFER_DST_OPTIMAL,             VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL },
            {ImageLayout::PRESENT_SRC,                      VkImageLayout::VK_IMAGE_LAYOUT_PRESENT_SRC_KHR }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E

    // reserved
    VK_CONVERT_INSTANCE_B(ShaderStageBits, VkShaderStageFlagBits)
        static std::unordered_map<ShaderStageBits, VkShaderStageFlagBits> MAP = {
            { ShaderStageBits::VERTEX, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT },
            { ShaderStageBits::FRAGMENT, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT },
            { ShaderStageBits::COMPUTE, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT }
        };
        FIND_OR_EXCEPT
    VK_CONVERT_INSTANCE_E
}

namespace Explosion::RHI {
    VK_GET_FLAGS_INSTANCE_B(ShaderStageBits, VkShaderStageFlagBits)
        static std::unordered_map<ShaderStageBits, VkShaderStageFlagBits> MAP = {
            { ShaderStageBits::VERTEX, VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT },
            { ShaderStageBits::FRAGMENT, VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT },
            { ShaderStageBits::COMPUTE, VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT }
        };
        COMBINE_FLAGS_AND_RETURN
    VK_GET_FLAGS_INSTANCE_E

    VK_GET_FLAGS_INSTANCE_B(CullModeBits, VkCullModeFlagBits)
        static std::unordered_map<CullModeBits, VkCullModeFlagBits> MAP = {
            { CullModeBits::NONE, VkCullModeFlagBits::VK_CULL_MODE_NONE },
            { CullModeBits::FRONT, VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT },
            { CullModeBits::BACK, VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT }
        };
        COMBINE_FLAGS_AND_RETURN
    VK_GET_FLAGS_INSTANCE_E

    VK_GET_FLAGS_INSTANCE_B(BufferUsageBits, VkBufferUsageFlagBits)
        static std::unordered_map<BufferUsageBits, VkBufferUsageFlagBits> MAP = {
            {BufferUsageBits::VERTEX_BUFFER,  VkBufferUsageFlagBits::VK_BUFFER_USAGE_VERTEX_BUFFER_BIT },
            {BufferUsageBits::INDEX_BUFFER,   VkBufferUsageFlagBits::VK_BUFFER_USAGE_INDEX_BUFFER_BIT },
            {BufferUsageBits::UNIFORM_BUFFER, VkBufferUsageFlagBits::VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT },
            {BufferUsageBits::STORAGE_BUFFER, VkBufferUsageFlagBits::VK_BUFFER_USAGE_STORAGE_BUFFER_BIT },
            {BufferUsageBits::TRANSFER_SRC,   VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_SRC_BIT },
            {BufferUsageBits::TRANSFER_DST,   VkBufferUsageFlagBits::VK_BUFFER_USAGE_TRANSFER_DST_BIT }
        };
        COMBINE_FLAGS_AND_RETURN
    VK_GET_FLAGS_INSTANCE_E

    VK_GET_FLAGS_INSTANCE_B(PipelineStageBits, VkPipelineStageFlagBits)
        static std::unordered_map<PipelineStageBits, VkPipelineStageFlagBits> MAP = {
            { PipelineStageBits::COLOR_ATTACHMENT_OUTPUT, VkPipelineStageFlagBits::VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT }
        };
        COMBINE_FLAGS_AND_RETURN
    VK_GET_FLAGS_INSTANCE_E

    VK_GET_FLAGS_INSTANCE_B(MemoryPropertyBits, VkMemoryPropertyFlagBits)
        static std::unordered_map<MemoryPropertyBits, VkMemoryPropertyFlagBits> MAP = {
            { MemoryPropertyBits::DEVICE_LOCAL, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT },
            { MemoryPropertyBits::HOST_VISIBLE, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT },
            { MemoryPropertyBits::HOST_COHERENT, VkMemoryPropertyFlagBits::VK_MEMORY_PROPERTY_HOST_COHERENT_BIT }
        };
        COMBINE_FLAGS_AND_RETURN
    VK_GET_FLAGS_INSTANCE_E

    VK_GET_FLAGS_INSTANCE_B(ImageUsageBits, VkImageUsageFlagBits)
        static std::unordered_map<ImageUsageBits, VkImageUsageFlagBits> MAP = {
            { ImageUsageBits::TRANSFER_SRC, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_SRC_BIT },
            { ImageUsageBits::TRANSFER_DST, VkImageUsageFlagBits::VK_IMAGE_USAGE_TRANSFER_DST_BIT },
            { ImageUsageBits::COLOR_ATTACHMENT, VkImageUsageFlagBits::VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT },
            { ImageUsageBits::DEPTH_STENCIL_ATTACHMENT, VkImageUsageFlagBits::VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT }
        };
        COMBINE_FLAGS_AND_RETURN
    VK_GET_FLAGS_INSTANCE_E

    VK_GET_FLAGS_INSTANCE_B(ImageAspectBits, VkImageAspectFlagBits)
        static std::unordered_map<ImageAspectBits, VkImageAspectFlagBits> MAP = {
            { ImageAspectBits::COLOR, VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT },
            { ImageAspectBits::DEPTH, VkImageAspectFlagBits::VK_IMAGE_ASPECT_DEPTH_BIT },
            { ImageAspectBits::STENCIL, VkImageAspectFlagBits::VK_IMAGE_ASPECT_STENCIL_BIT }
        };
        COMBINE_FLAGS_AND_RETURN
    VK_GET_FLAGS_INSTANCE_E
}
