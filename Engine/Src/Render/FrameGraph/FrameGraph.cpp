//
// Created by LiZhen on 2021/5/9.
//

#include <Explosion/Common/Exception.h>
#include <Explosion/Render/FrameGraph/FrameGraph.h>
#include <Explosion/Driver/Driver.h>

namespace Explosion {

    FgHandle FrameGraphBuilder::Read(FgHandle handle)
    {
        auto index = handle.Index();
        EXPLOSION_ASSERT(!!handle || index >= graph.resources.size(), "Invalid Frame graph Handle");
        FgNode* from = graph.resources[index].get();
        FgNode* to = renderPass;
        graph.edges.emplace_back(FrameGraph::Edge{from, to});
        auto& vector = graph.incomingEdgeMap[to];
        vector.emplace_back(from);
        return handle;
    }

    FgHandle FrameGraphBuilder::Write(FgHandle handle)
    {
        auto index = handle.Index();
        EXPLOSION_ASSERT(!!handle || index >= graph.resources.size(), "Invalid Frame graph Handle");
        graph.resHandles[index] = ++handle;  // version++

        FgNode* from = renderPass;
        FgNode* to = graph.resources[index].get();
        graph.edges.emplace_back(FrameGraph::Edge{from, to});
        auto& vector = graph.incomingEdgeMap[to];
        vector.emplace_back(from);
        return handle;
    }

    void FrameGraphBuilder::SideEffect()
    {
        renderPass->SideEffect();
    }

    FrameGraph& FrameGraph::Compile()
    {
        Cull();
        return *this;
    }

    FrameGraph& FrameGraph::Execute(Driver& driver)
    {
        return *this;
    }

    /**
     * Step1.
     *   pass.RefCount++ for every resource write
     *   res.RefCount++ for every resource read
     *   Equals directed edge begin node refCount++
     * Step2.
     *   Identify resources with refCount == 0 -> stack
     * Step3.
     *   while stack is non-empty
     *   Pop a resource and decrement refCount of its producer
     *   if producer.refCount == 0
     *     decrement refCount of resources tha it reads
     *     Add them to the satck when refCount == 0
     */
    void FrameGraph::Cull()
    {
        for (auto& edge : edges) {
            edge.from->AddRef();
        }
        std::vector<FgNode*> stack;
        for (auto& res : resources) {
            if (!res) stack.emplace_back(res.get());
        }
        for (auto& pass : passes) {
            if (!pass) stack.emplace_back(pass.get());
        }

        while (!stack.empty()) {
            FgNode* node = stack.back();
            stack.pop_back();
            auto& vector = incomingEdgeMap[node];
            for (auto& in : vector) {
                in->RemoveRef();
                if (!in) stack.emplace_back(in);
            }
        }
    }

}
