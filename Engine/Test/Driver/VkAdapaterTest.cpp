//
// Created by John Kindem on 2021/5/9 0009.
//

#include <gtest/gtest.h>

#include <Explosion/Driver/Enum.h>
#include <Explosion/Driver/VkAdapater.h>

TEST(VkAdapaterTest, VkBoolConvertTest)
{
    ASSERT_EQ(Explosion::VkBoolConvert(true), VK_TRUE);
    ASSERT_EQ(Explosion::VkBoolConvert(false), VK_FALSE);
}

TEST(VkAdapterTest, VkConvertTest)
{
    VkFormat format = Explosion::VkConvert<Explosion::Format, VkFormat>(Explosion::Format::R32_G32_B32_FLOAT);
    ASSERT_EQ(format, VK_FORMAT_R32G32B32_SFLOAT);
}

TEST(VkAdapterTest, GetEnumByVkTest)
{
    Explosion::Format format = Explosion::GetEnumByVk<VkFormat, Explosion::Format>(VK_FORMAT_R32G32B32_SFLOAT);
    ASSERT_EQ(format, Explosion::Format::R32_G32_B32_FLOAT);
}

TEST(VkAdapterTest, VkGetFlagsTest)
{
    std::vector<Explosion::BufferUsage> bufferUsages = {
        Explosion::BufferUsage::VERTEX_BUFFER,
        Explosion::BufferUsage::TRANSFER_DST
    };
    VkFlags targetFlags = Explosion::VkGetFlags<Explosion::BufferUsage, VkBufferUsageFlagBits>(bufferUsages);
    ASSERT_EQ(targetFlags, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
}
