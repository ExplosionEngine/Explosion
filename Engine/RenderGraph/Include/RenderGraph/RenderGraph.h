//
// Created by johnk on 2021/12/20.
//

#ifndef EXPLOSION_RENDER_GRAPH_H
#define EXPLOSION_RENDER_GRAPH_H

#include <cstdint>
#include <string>
#include <vector>
#include <memory>

#include <Common/Utility.h>

namespace RenderGraph {
    template <typename T>
    using Handle = uint64_t;

    template <typename T>
    const Handle<T> HANDLE_NONE = -1;

    template <typename T1, typename T2>
    Handle<T1> HandleCast(Handle<T2> handle)
    {
        return static_cast<Handle<T1>>(handle);
    }

    class Node {
    public:
        NON_COPYABLE(Node)
        explicit Node(std::string name) : name(std::move(name)) {}
        virtual ~Node() = 0;

        const std::string& GetName()
        {
            return name;
        }

    private:
        std::string name;
    };

    class ResourceNode : public Node {
    public:
        NON_COPYABLE(ResourceNode)
        explicit ResourceNode(std::string name) : Node(std::move(name)) {}
        ~ResourceNode() override = default;

    private:
        uint64_t refCount = 0;
    };

    class PassNode : public Node {
    public:
        NON_COPYABLE(PassNode)
        explicit PassNode(std::string name) : Node(std::move(name)) {}
        ~PassNode() override = default;

    private:
        std::vector<Handle<Node>> readResources;
        std::vector<Handle<Node>> writeResources;
    };

    class RenderGraphBuilder;

    class RenderGraph {
    public:
        NON_COPYABLE(RenderGraph)
        RenderGraph() = default;
        ~RenderGraph() = default;

    private:
        friend RenderGraphBuilder;

        std::vector<std::unique_ptr<Node>> nodes;
        std::vector<Handle<Node>> freeNodeSlots;
    };

    class RenderGraphBuilder {
    public:
        NON_COPYABLE(RenderGraphBuilder)
        explicit RenderGraphBuilder(RenderGraph& g) : graph(g) {}
        ~RenderGraphBuilder() = default;

        template <typename NodeType, typename... Args>
        Handle<NodeType> Create(Args&&... args)
        {
            static_assert(std::is_base_of_v<Node, NodeType>);
            if (graph.freeNodeSlots.empty()) {
                graph.nodes.template emplace_back(std::make_unique<NodeType>(std::forward<Args...>(args...)));
                return graph.nodes.size() - 1;
            } else {
                auto handle = *(graph.freeNodeSlots.end() - 1);
                graph.nodes[handle] = std::make_unique<NodeType>(std::forward<Args...>(args...));
                graph.freeNodeSlots.pop_back();
                return handle;
            }
        }

        template <typename PassNodeType, typename ResourceNodeType>
        bool ReadResource(Handle<PassNodeType> pass, Handle<ResourceNodeType> resource)
        {
            static_assert(std::is_base_of_v<PassNode, PassNodeType>);
            static_assert(std::is_base_of_v<ResourceNode, ResourceNodeType>);
            // TODO
        }

        template <typename PassNodeType, typename ResourceNodeType>
        bool WriteResource(Handle<PassNodeType> pass, Handle<ResourceNodeType> resource)
        {
            static_assert(std::is_base_of_v<PassNode, PassNodeType>);
            static_assert(std::is_base_of_v<ResourceNode, ResourceNodeType>);
            // TODO
        }

        template <typename NodeType>
        bool VerifyHandle(Handle<NodeType> handle)
        {
            return handle >= 0 && handle < graph.nodes.size();
        }

    private:
        RenderGraph& graph;
    };
}

#endif //EXPLOSION_RENDER_GRAPH_H
