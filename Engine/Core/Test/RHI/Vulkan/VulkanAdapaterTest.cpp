//
// Created by John Kindem on 2021/5/9 0009.
//

#include <gtest/gtest.h>

#include <Engine/RHI/Common/Enum.h>
#include <Engine/RHI/Vulkan/VulkanAdapater.h>

using namespace Explosion::RHI;

TEST(VulkanAdapterTest, VkBoolConvertTest)
{
    ASSERT_EQ(VkBoolConvert(true), VK_TRUE);
    ASSERT_EQ(VkBoolConvert(false), VK_FALSE);
}

TEST(VulkanAdapterTest, VkConvertTest)
{
    VkFormat format = VkConvert<Format, VkFormat>(Format::R32_G32_B32_FLOAT);
    ASSERT_EQ(format, VK_FORMAT_R32G32B32_SFLOAT);

    format = VkConvert<Format, VkFormat>(Format::R8_G8_B8_A8_RGB);
    ASSERT_EQ(format, VK_FORMAT_R8G8B8A8_SRGB);

    format = VkConvert<Format, VkFormat>(Format::R32_G32_UNSIGNED_INT);
    ASSERT_EQ(format, VK_FORMAT_R32G32_UINT);
}

TEST(VulkanAdapterTest, GetEnumByVkTest)
{
    Format format = GetEnumByVk<VkFormat, Format>(VK_FORMAT_R32G32B32_SFLOAT);
    ASSERT_EQ(format, Format::R32_G32_B32_FLOAT);

    format = GetEnumByVk<VkFormat, Format>(VK_FORMAT_R32G32_SINT);
    ASSERT_EQ(format, Format::R32_G32_SIGNED_INT);

    format = GetEnumByVk<VkFormat, Format>(VK_FORMAT_R8G8B8A8_SRGB);
    ASSERT_EQ(format, Format::R8_G8_B8_A8_RGB);
}

TEST(VulkanAdapterTest, VkGetFlagsTest)
{
    BufferUsageFlags bufferUsageFlags = BufferUsageBits::VERTEX_BUFFER | BufferUsageBits::TRANSFER_DST;
    VkFlags targetFlags = VkGetFlags<BufferUsageBits, VkBufferUsageFlagBits>(bufferUsageFlags);
    ASSERT_EQ(targetFlags, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);

    ShaderStageFlags shaderStageFlags = ShaderStageBits::VERTEX | ShaderStageBits::FRAGMENT;
    targetFlags = VkGetFlags<ShaderStageBits, VkShaderStageFlagBits>(shaderStageFlags);
    ASSERT_EQ(targetFlags, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT);
}
