//
// Created by johnk on 2023/12/11.
//

#include <Test/Test.h>

#include <Render/ResourcePool.h>

using namespace Render;

struct ResourcePoolTest : testing::Test {
    void SetUp() override
    {
        instance = RHI::Instance::GetByType(RHI::RHIType::dummy);

        device = instance->GetGpu(0)->RequestDevice(
            RHI::DeviceCreateInfo()
                .AddQueueRequest(RHI::QueueRequestInfo(RHI::QueueType::graphics, 1)));
    }

    void TearDown() override {}

    RHI::Instance* instance;
    Common::UniqueRef<RHI::Device> device;
};

TEST_F(ResourcePoolTest, BasicTest)
{
    auto& texturePool = TexturePool::Get(*device);
    PooledTextureDesc textureDesc {};
    textureDesc.dimension = RHI::TextureDimension::t2D;
    textureDesc.width = 1920;
    textureDesc.height = 1080;
    textureDesc.depthOrArraySize = 1;
    textureDesc.format = RHI::PixelFormat::rgba8Unorm;
    textureDesc.usages = RHI::TextureUsageBits::renderAttachment | RHI::TextureUsageBits::storageBinding;
    textureDesc.mipLevels = 1;
    textureDesc.samples = 1;
    textureDesc.initialState = RHI::TextureState::undefined;
    PooledTextureRef t1 = texturePool.Allocate(textureDesc);

    textureDesc.width = 1024;
    textureDesc.height = 1024;
    textureDesc.depthOrArraySize = 1;
    PooledTextureRef t2 = texturePool.Allocate(textureDesc);

    textureDesc.width = 1920;
    textureDesc.height = 1080;
    textureDesc.depthOrArraySize = 1;
    const PooledTextureRef t3 = texturePool.Allocate(textureDesc);
    ASSERT_NE(t1.Get(), t3.Get());

    auto* texturePtr = t1.Get();
    t1.Reset();
    const PooledTextureRef t4 = texturePool.Allocate(textureDesc);
    ASSERT_EQ(texturePtr, t4.Get());
}
