//
// Created by John Kindem on 2021/6/10.
//

#ifndef EXPLOSION_ECS_H
#define EXPLOSION_ECS_H

#include <memory>

#include <entt/entt.hpp>

#include <Explosion/JobSystem/JobSystem.h>

namespace Explosion::ECS {
    using Entity = entt::entity;

    class Registry;

    template <typename... T>
    using ExcludeT = entt::exclude_t<T...>;

    template <typename... T>
    class View {
    public:
        ~View() = default;

        template <typename Func>
        void Each(const Func& func)
        {
            return view.each(func);
        }

    private:
        friend Registry;

        explicit View(entt::view<T...> view) : view(std::move(view)) {}

        entt::view<T...> view;
    };

    class Registry {
    public:
        Registry() = default;
        ~Registry() = default;

        Entity CreateEntity()
        {
            return registry.create();
        }

        void DestroyEntity(Entity entity)
        {
            registry.destroy(entity);
        }

        template <typename Comp, typename... Args>
        void AddComponent(Entity entity, Args&&... args)
        {
            registry.template emplace_or_replace<Comp>(entity, args...);
        }

        template <typename Comp>
        Comp* GetComponent(Entity entity)
        {
            return registry.template try_get<Comp>(entity);
        }

        template <typename... Comp, typename... Exclude>
        View<ExcludeT<Exclude...>, Comp...> CreateView(const ExcludeT<Exclude...>& excludes = {})
        {
            return View<ExcludeT<Exclude...>, Comp...>(registry.template view<Comp...>(excludes));
        }

    private:
        entt::registry registry;
    };

    using System = std::function<JobSystem::TaskFlow(Registry& registry, float time)>;

    struct SystemNode {
        std::string name {};
        std::vector<std::unique_ptr<SystemNode>> afters {};
    };

    struct SystemGraph {
        std::vector<std::unique_ptr<SystemNode>> roots;
    };

    class SystemGraphBuilder {
    public:
        SystemGraphBuilder() = default;
        ~SystemGraphBuilder() = default;

        SystemGraphBuilder& Emplace(const std::string& name)
        {
            if (auto iter = nodes.find(name); iter != nodes.end()) {
                throw std::runtime_error("specific system node already exists in graph");
            }
            nodes[name] = std::make_unique<SystemNode>();
            nodes[name]->name = name;
            return *this;
        }

        SystemGraphBuilder& Emplace(const std::string& name, const std::string& last)
        {
            if (auto iter = nodes.find(last); iter == nodes.end()) {
                throw std::runtime_error("there is no system node with specific name");
            }
            Emplace(name);
            dependencies[name] = last;
            return *this;
        }

        SystemGraph Build()
        {
            for (auto& dependency : dependencies) {
                nodes[dependency.second]->afters.emplace_back(std::move(nodes[dependency.first]));
                nodes.erase(nodes.find(dependency.first));
            }

            SystemGraph systemGraph;
            for (auto& node : nodes) {
                systemGraph.roots.emplace_back(std::move(node.second));
            }
            return systemGraph;
        }

    private:
        std::unordered_map<std::string, std::unique_ptr<SystemNode>> nodes;
        std::unordered_map<std::string, std::string> dependencies;
    };

    struct SystemGroup {
        std::string name;
        uint32_t priority;
        std::unordered_map<std::string, System> systems;
        SystemGraph systemGraph;
    };
}

#endif //EXPLOSION_ECS_H
