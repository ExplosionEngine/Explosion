//
// Created by LiZhen on 2021/5/12.
//

#include <gtest/gtest.h>
#include <Explosion/RHI/Common/Driver.h>
#include <Explosion/Render/FrameGraph/FgResources.h>
#include <Explosion/Render/FrameGraph/FrameGraph.h>

using namespace Explosion;

TEST(FrameGraphTest, FrameGraphResourceHandleTest1)
{
    FgResourceHandle res0;
    ASSERT_EQ(!!res0, false);

    FgResourceHandle res1(0);
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 0);

    ++res1;
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 1);

    res1.Reset();
    ASSERT_EQ(!!res1, false);
}

TEST(FrameGraphTest, FrameGraphResourceHandleTest2)
{
    FgResourceHandle<uint64_t> res0;
    ASSERT_EQ(!!res0, false);

    FgResourceHandle res1(0);
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 0);

    ++res1;
    ASSERT_EQ(!!res1, true);
    ASSERT_EQ(res1.Index(), 0);
    ASSERT_EQ(res1.Version(), 1);

    res1.Reset();
    ASSERT_EQ(!!res1, false);
}

// with side effect.
TEST(FrameGraphTest, FrameGraphCreateTest1)
{
    FrameGraph graph;
    struct PassData {
        FgHandle attachment;
    };

    FgHandle attachmentHandle;

    graph.AddCallbackPass<PassData>("test", [&](FrameGraphBuilder& builder, PassData& data) {
        auto tex = builder.CreateSubResource<FgTexture>("test",
                                                        FgTexture::Descriptor{1, 1, 0});
        attachmentHandle = data.attachment = builder.Write(tex);
    }, [=](const PassData& data, const FrameGraph& graph, RHI::Driver& driver) {
    });

    graph.Compile();

    ASSERT_EQ(graph.GetPasses()[0]->IsActive(), false);
}

TEST(FrameGraphTest, FrameGraphCreateTest2)
{
    FrameGraph graph;
    struct PassData1 {
        FgHandle output;
    };

    struct PassData2 {
        FgHandle input;
    };

    FgHandle attachmentHandle;

    graph.AddCallbackPass<PassData1>("test", [&](FrameGraphBuilder& builder, PassData1& data) {
        auto tex = builder.CreateSubResource<FgTexture>("test", FgTexture::Descriptor{1, 1, 0});
        attachmentHandle = data.output = builder.Write(tex);
    }, [=](const PassData1& data, const FrameGraph& graph, RHI::Driver& driver) {
    });

    graph.AddCallbackPass<PassData2>("present", [&](FrameGraphBuilder& builder, PassData2& data) {
        data.input = builder.Read(attachmentHandle);
    }, [=](const PassData2& data, const FrameGraph& graph, RHI::Driver& driver) {
    });
    graph.Compile();

    ASSERT_EQ(graph.GetPasses()[0]->IsActive(), false);
    ASSERT_EQ(graph.GetPasses()[1]->IsActive(), false);
}

TEST(FrameGraphTest, FrameGraphCreateTest3)
{
    FrameGraph graph;
    struct PassData1 {
        FgHandle output;
    };

    struct PassData2 {
        FgHandle input;
    };

    FgHandle attachmentHandle;

    graph.AddCallbackPass<PassData1>("test", [&](FrameGraphBuilder& builder, PassData1& data) {
        auto tex = builder.CreateSubResource<FgTexture>("test", FgTexture::Descriptor{1, 1, 0});
        attachmentHandle = data.output = builder.Write(tex);
    }, [=](const PassData1& data, const FrameGraph& graph, RHI::Driver& driver) {
    });

    graph.AddCallbackPass<PassData2>("present", [&](FrameGraphBuilder& builder, PassData2& data) {
        data.input = builder.Read(attachmentHandle);
        builder.SideEffect();
    }, [=](const PassData2& data, const FrameGraph& graph, RHI::Driver& driver) {
    });
    graph.Compile();

    ASSERT_EQ(graph.GetPasses()[0]->IsActive(), true);
    ASSERT_EQ(graph.GetPasses()[1]->IsActive(), true);
}