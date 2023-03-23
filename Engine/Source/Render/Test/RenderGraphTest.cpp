//
// Created by johnk on 2023/3/22.
//

#include <gtest/gtest.h>

#include <RHI/RHI.h>
#include <Render/RenderGraph.h>

struct RenderGraphTest : public testing::Test {
    void SetUp() override
    {
        instance = RHI::Instance::GetByType(RHI::RHIType::DUMMY);

        RHI::QueueInfo queueInfo {};
        queueInfo.type = RHI::QueueType::GRAPHICS;
        queueInfo.num = 1;
        RHI::DeviceCreateInfo deviceCreateInfo {};
        deviceCreateInfo.queueCreateInfoNum = 1;
        deviceCreateInfo.queueCreateInfos = &queueInfo;
        device = instance->GetGpu(0)->RequestDevice(&deviceCreateInfo);
    }

    void TearDown() override {}

    RHI::Instance* instance;
    RHI::UniqueRef<RHI::Device> device;
};

TEST_F(RenderGraphTest, BasicTest)
{
    RHI::UniqueRef<RHI::Fence> mainFence = device->CreateFence();

    Render::RenderGraph renderGraph(*device);
    renderGraph.Setup();
    renderGraph.Compile();
    renderGraph.Execute(mainFence.Get(), nullptr);
    mainFence->Wait();
}
