//
// Created by LiZhen on 2021/5/9.
//

#ifndef EXPLOSION_FRAMEGRAPH_H
#define EXPLOSION_FRAMEGRAPH_H

#include <vector>
#include <memory>
#include <unordered_map>
#include <Explosion/Common/NonCopy.h>
#include <Explosion/Common/Exception.h>
#include <Explosion/Render/FrameGraph/FgRenderPass.h>
#include <Explosion/Render/FrameGraph/FgResources.h>
#include <Explosion/Render/FrameGraph/FgBlackBoard.h>

namespace Explosion {
    class FrameGraph;

    class FrameGraphBuilder {
    public:
        explicit FrameGraphBuilder(FrameGraph& g, FgRenderPassBase* pass) : graph(g), renderPass(pass) {}

        ~FrameGraphBuilder() = default;

        template <typename Resource>
        FgHandle Create(const char* name, const typename Resource::Descriptor& desc);

        template <typename Resource>
        FgHandle CreateSubResource(const char* name, const typename Resource::Descriptor& desc);

        template <typename Resource>
        FgHandle CreateSubResource(const char* name, FgHandle parent,
                                   const typename Resource::SubResource::Descriptor& desc);

        FgHandle Read(FgHandle handle);

        FgHandle Write(FgHandle handle);

        FgBlackBoard& GetBlackBoard() { return blackboard; }

        void SideEffect();

    private:
        FrameGraph& graph;
        FgBlackBoard blackboard;
        FgRenderPassBase* renderPass;
    };

    class FrameGraph {
    public:
        FrameGraph() = default;

        ~FrameGraph() = default;

        EXPLOSION_NON_COPY(FrameGraph);

        template<typename Data, typename Setup, typename Execute>
        void AddCallbackPass(const char* name, Setup &&s, Execute &&e)
        {
            auto pass = new FgRenderPass<Data>(name, std::forward<Execute>(e));
            passes.emplace_back(pass);
            FrameGraphBuilder builder(*this, pass);
            s(builder, pass->data);
        }

        FrameGraph& Compile();

        FrameGraph& Execute(RHI::Driver&);

        using PassPtr = std::unique_ptr<FgRenderPassBase>;
        using ResPtr = std::unique_ptr<FgVirtualResource>;

        using PassVector = std::vector<PassPtr>;
        using ResourceVector = std::vector<ResPtr>;

        [[nodiscard]] const PassVector& GetPasses() const { return passes; }

        [[nodiscard]] const ResourceVector& GetResources() const { return resources; }

        const FgVirtualResource* GetResource(FgHandle handle) const;

    private:
        struct Edge {
            FgNode* from;
            FgNode* to;
        };

        using PassIter = PassVector::const_iterator;

        void PerformCulling();

        PassIter ReOrderRenderPass();

        friend class FrameGraphBuilder;
        PassVector passes;
        ResourceVector resources;
        std::vector<FgHandle> resHandles;
        std::vector<Edge> edges;
        std::unordered_map<FgNode*, std::vector<FgNode*>> incomingEdgeMap;
    };


    template <typename Resource>
    FgHandle FrameGraphBuilder::Create(const char* name, const typename Resource::Descriptor& desc)
    {
        auto res = new Resource(name, desc);
        FgHandle handle(static_cast<FgHandle::HandleType>(graph.resources.size()));
        graph.resources.emplace_back(res);
        graph.resHandles.emplace_back(handle);
        return handle;
    }

    template <typename Resource>
    FgHandle FrameGraphBuilder::CreateSubResource(const char* name, const typename Resource::Descriptor& desc)
    {
        FgHandle res = Create<Resource>(name, desc);
        return CreateSubResource<Resource>(name, res, SubResourceTrait<Resource>().Init());
    }

    template <typename Resource>
    FgHandle FrameGraphBuilder::CreateSubResource(const char* name, FgHandle parent,
                                                  const typename Resource::SubResource::Descriptor& desc)
    {
        EXPLOSION_ASSERT(parent.Index() < graph.resources.size(), "invalid subresource parent");
        FgHandle handle(static_cast<FgHandle::HandleType>(graph.resources.size()));
        auto parentRes = static_cast<Resource*>(graph.resources[parent.Index()].get());
        EXPLOSION_ASSERT(parentRes != nullptr, "invalid subresource parent");
        auto res = new typename Resource::SubResource(name, *parentRes, desc);
        graph.resources.emplace_back(res);
        graph.resHandles.emplace_back(handle);
        return handle;
    }
}

#endif //EXPLOSION_FRAMEGRAPH_H
