//
// Created by johnk on 2021/12/20.
//

#ifndef EXPLOSION_RENDER_GRAPH_H
#define EXPLOSION_RENDER_GRAPH_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <unordered_map>

#include <Common/Utility.h>

namespace RenderGraph {
    class RenderGraphBuilder;
    class RenderGraphPassBuilder;
    class RenderGraphCuller;
    class InstancedResources;

    using PassExecutor = std::function<void(InstancedResources&)>;
    using PassSetup = std::function<PassExecutor(RenderGraphPassBuilder&)>;

    class VirtualResource {};
    class Texture : public VirtualResource {};
    class Buffer : public VirtualResource {};

    class GlobalTexture : public Texture {};
    class GlobalBuffer : public Buffer {};

    class InstancedResource {};
    class InstancedTexture : public InstancedResource {};
    class InstancedBuffer : public InstancedResource {};

    template <typename VR>
    struct VirtualResourceTraits {};

    template <typename VR>
    using GetIRTypeByVR = typename VirtualResourceTraits<VR>::IR;

    template <>
    struct VirtualResourceTraits<Texture> {
        using IR = InstancedTexture;
    };

    template <>
    struct VirtualResourceTraits<Buffer> {
        using IR = InstancedBuffer;
    };

    class RenderGraph {
    public:
        NON_COPYABLE(RenderGraph)
        RenderGraph() = default;
        ~RenderGraph() = default;

    private:
        friend RenderGraphBuilder;
        friend RenderGraphPassBuilder;
        friend RenderGraphCuller;

        struct Pass {
            std::string name;
            PassSetup setup;
            std::vector<VirtualResource*> reads;
            std::vector<VirtualResource*> writes;
        };

        std::vector<std::unique_ptr<Pass>> passes;
        std::vector<std::unique_ptr<VirtualResource>> virtualResources;
    };

    class RenderGraphBuilder {
    public:
        NON_COPYABLE(RenderGraphBuilder)
        explicit RenderGraphBuilder(RenderGraph& g) : graph(g) {}
        ~RenderGraphBuilder() = default;

        void AddPass(const std::string& name, PassSetup&& setup)
        {
            // TODO
        }

        template <typename VR, typename... Args>
        VR* CreateVirtualResource(const std::string& name, Args&&... args)
        {
            // TODO
        }

        template <typename VR, typename... Args>
        VR* RegisterGlobalResource(const std::string& name, Args&&... args)
        {
            // TODO
        }

    private:
        RenderGraph& graph;
    };

    class RenderGraphPassBuilder {
    public:
        NON_COPYABLE(RenderGraphPassBuilder)
        explicit RenderGraphPassBuilder(RenderGraph& g, RenderGraph::Pass& p) : graph(g), pass(p) {}
        ~RenderGraphPassBuilder() = default;

        void ReadResource(VirtualResource* virtualResource)
        {
            // TODO
        }

        void WriteResource(VirtualResource* virtualResource)
        {
            // TODO
        }

    private:
        RenderGraph& graph;
        RenderGraph::Pass& pass;
    };

    class RenderGraphCuller {
    public:
        NON_COPYABLE(RenderGraphCuller)
        explicit RenderGraphCuller(RenderGraph& g) : graph(g) {}
        ~RenderGraphCuller() = default;

        void CullGraph()
        {
            // TODO
        }

    private:
        RenderGraph& graph;
    };

    class InstancedResources {
    public:
        template <typename VR>
        GetIRTypeByVR<VR>* GetInstancedResource(VR* virtualResource)
        {
            // TODO
        }

    private:
        std::unordered_map<VirtualResource*, InstancedResource*> instancedResources;
    };
}

#endif //EXPLOSION_RENDER_GRAPH_H