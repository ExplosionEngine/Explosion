//
// Created by John Kindem on 2021/5/9 0009.
//

#include <gtest/gtest.h>

#include <Explosion/RHI/Common/Enum.h>
#include <Explosion/RHI/Vulkan/VulkanAdapater.h>

using namespace Explosion::RHI;

TEST(VkAdapaterTest, VkBoolConvertTest)
{
    ASSERT_EQ(VkBoolConvert(true), VK_TRUE);
    ASSERT_EQ(VkBoolConvert(false), VK_FALSE);
}

TEST(VkAdapterTest, VkConvertTest)
{
    VkFormat format = VkConvert<Format, VkFormat>(Format::R32_G32_B32_FLOAT);
    ASSERT_EQ(format, VK_FORMAT_R32G32B32_SFLOAT);
}

TEST(VkAdapterTest, GetEnumByVkTest)
{
    Format format = GetEnumByVk<VkFormat, Format>(VK_FORMAT_R32G32B32_SFLOAT);
    ASSERT_EQ(format, Format::R32_G32_B32_FLOAT);
}

TEST(VkAdapterTest, VkGetFlagsTest)
{
    BufferUsageFlags bufferUsageFlags = BufferUsage::VERTEX_BUFFER | BufferUsage::TRANSFER_DST;
    VkFlags targetFlags = VkGetFlags<BufferUsage, VkBufferUsageFlagBits>(bufferUsageFlags);
    ASSERT_EQ(targetFlags, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}
