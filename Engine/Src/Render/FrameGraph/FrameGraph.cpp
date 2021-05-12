//
// Created by LiZhen on 2021/5/9.
//

#include <Explosion/Common/Exception.h>
#include <Explosion/Render/FrameGraph/FrameGraph.h>
#include <Explosion/Driver/Driver.h>

namespace Explosion {

    FgHandle FrameGraphBuilder::Read(FgHandle handle)
    {
        return {};
    }

    FgHandle FrameGraphBuilder::Write(FgHandle handle)
    {
        auto index = handle.Index();
        EXPLOSION_ASSERT(!!handle || index >= graph.resources.size(), "Invalid Frame graph Handle");
        graph.resHandles[index] = ++handle;
        graph.edges.emplace_back(FrameGraph::Edge{renderPass, graph.resources[index].get()});
        return handle;
    }

    FrameGraph& FrameGraph::Compile()
    {
        return *this;
    }

    FrameGraph& FrameGraph::Execute(Driver& driver)
    {
        return *this;
    }

}
