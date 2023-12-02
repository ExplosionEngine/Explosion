//
// Created by johnk on 2023/3/22.
//

#include <gtest/gtest.h>

#include <Rendering/RenderGraph.h>
#include <Rendering/Pipeline.h>
#include <Render/Shader.h>

struct RenderGraphTest : public testing::Test {
    void SetUp() override
    {
        instance = RHI::Instance::GetByType(RHI::RHIType::dummy);

        RHI::QueueInfo queueInfo {};
        queueInfo.type = RHI::QueueType::graphics;
        queueInfo.num = 1;
        RHI::DeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.queueCreateInfoNum = 1;
        deviceCreateInfo.queueCreateInfos = &queueInfo;
        device = instance->GetGpu(0)->RequestDevice(deviceCreateInfo);
    }

    void TearDown() override {}

    RHI::Instance* instance;
    Common::UniqueRef<RHI::Device> device;
};

TEST_F(RenderGraphTest, BasicTest)
{
    Common::UniqueRef<RHI::Fence> mainFence = device->CreateFence();
    // TODO
}
