//
// Created by John Kindem on 2021/5/20.
//

#include <gtest/gtest.h>

#include <Explosion/RHI/Common/Enum.h>

using namespace Explosion::RHI;

TEST(EnumTest, FlagsCastTest)
{
    ASSERT_EQ(FlagsCast(ShaderStageBits::VERTEX), 0x1);
    ASSERT_EQ(FlagsCast(ShaderStageBits::FRAGMENT), 0x2);

    ASSERT_EQ(FlagsCast(BufferUsageBits::STORAGE_BUFFER), 0x8);
    ASSERT_EQ(FlagsCast(BufferUsageBits::TRANSFER_DST), 0x20);
}

TEST(EnumTest, OperatorAndTest)
{
    ASSERT_EQ(ShaderStageBits::VERTEX & ShaderStageBits::FRAGMENT, 0x1 & 0x2);
    ASSERT_EQ(ShaderStageBits::VERTEX & ShaderStageBits::VERTEX, 0x1 & 0x1);
}

TEST(EnumTest, OperatorOrTest)
{
    ASSERT_EQ(BufferUsageBits::VERTEX_BUFFER | BufferUsageBits::INDEX_BUFFER, 0x1 | 0x2);
    ASSERT_EQ(BufferUsageBits::INDEX_BUFFER | BufferUsageBits::TRANSFER_DST, 0x2 | 0x20);
}
