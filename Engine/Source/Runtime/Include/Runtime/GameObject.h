//
// Created by johnk on 2024/6/27.
//

#pragma once

#include <Runtime/Component.h>

namespace Runtime {
    class EClass() GameObject {
        EClassBody(GameObject)

        template <typename C, typename... Args>
        C& Emplace(Args&&... args);

        template <typename C>
        bool Has();

        template <typename C>
        C* Get();

        template <typename C>
        void Remove();

    private:
        EProperty()
        std::unordered_map<std::string, ComponentStorage> components;
    };
}

namespace Runtime {
    template <typename C, typename ... Args>
    C& GameObject::Emplace(Args&&... args)
    {
        const Mirror::Class& clazz = Mirror::Class::Get<C>();
        const auto& className = clazz.GetName();
        Assert(!components.contains(className));

        ComponentStorage storage;
        storage.clazz = &clazz;
        storage.storage = C(std::forward<Args>(args)...);
        components.emplace(className, std::move(storage));
        return Get<C>;
    }

    template <typename C>
    bool GameObject::Has()
    {
        const Mirror::Class& clazz = Mirror::Class::Get<C>();
        const auto& className = clazz.GetName();
        return components.contains(className);
    }

    template <typename C>
    C* GameObject::Get()
    {
        const Mirror::Class& clazz = Mirror::Class::Get<C>();
        const auto& className = clazz.GetName();
        return Has<C>() ? nullptr : &components.at(className).storage.As<C&>();
    }

    template <typename C>
    void GameObject::Remove()
    {
        const Mirror::Class& clazz = Mirror::Class::Get<C>();
        const auto& className = clazz.GetName();
        components.erase(className);
    }
}

#define DeclareGameObjectComponentScripts(componentName) \
    class EClass(scriptDelegate=GameObject) GameObjectComponentScripts_##componentName { \
    public: \
        explicit GameObjectComponentScripts_##componentName(GameObject& inObject); \
    \
        Mirror::Any Emplace(std::vector<Mirror::Any>& args) const \
        { \
            const auto object = Mirror::Class::Get<componentName>().ConstructOnStackSuitable(args.data(), args.size()); \
            return gameObject.Emplace<componentName>(object.As<const componentName&>()); \
        } \
    \
        Mirror::Any Has() const \
        { \
            return gameObject.Has<componentName>(); \
        } \
    \
        void Remove() const \
        { \
            gameObject.Remove<componentName>(); \
        } \
    \
    private: \
        GameObject& gameObject; \
    }; \
