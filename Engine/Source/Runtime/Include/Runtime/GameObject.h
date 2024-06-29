//
// Created by johnk on 2024/6/27.
//

// BEGIN_META_FORWARD_IMPL_HEADERS
// Runtime/World.h
// END_META_FORWARD_IMPL_HEADERS

#pragma once

#include <Runtime/Component.h>
#include <Common/Memory.h>
#include <Common/Serialization.h>

namespace Runtime {
    class World;

    class EClass() GameObject { // NOLINT
        EClassBody(GameObject)

        template <typename C, typename... Args>
        C& Emplace(Args&&... args);

        template <typename C>
        bool Has();

        template <typename C>
        C* Get();

        template <typename C>
        void Remove();

        ECtor() GameObject();
        ECtor() GameObject(World* inWorld, std::string inName);
        virtual ~GameObject();

        EFunc() World* GetWorld() const;

    protected:
        virtual void OnCreated();
        virtual void OnLoaded();
        virtual void OnDestroy();
        virtual void OnStart();
        virtual void OnTick(float frameTimeMs);
        virtual void OnStop();

    private:
        friend class World;

        void SetWorld(World* inWorld);

        EProperty(transient) World* world;
        EProperty() std::string name;
        EProperty() std::unordered_map<std::string, ComponentStorage> components;
    };

    using GameObjectRef = Common::SharedRef<GameObject>;
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
        return *Get<C>();
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
        return Has<C>() ? &components.at(className).storage.As<C&>() : nullptr;
    }

    template <typename C>
    void GameObject::Remove()
    {
        const Mirror::Class& clazz = Mirror::Class::Get<C>();
        const auto& className = clazz.GetName();
        components.erase(className);
    }
}

namespace Common { // NOLINT
    template <>
    struct Serializer<Runtime::GameObjectRef> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = HashUtils::StrCrc32("Runtime::GameObjectRef");

        static void Serialize(SerializeStream& stream, const Runtime::GameObjectRef& value)
        {
            TypeIdSerializer<Runtime::GameObjectRef>::Serialize(stream);

            Mirror::Any objectRef = std::ref(*value);
            Mirror::Class::Get<Runtime::GameObject>().Serialize(stream, &objectRef);
        }

        static bool Deserialize(DeserializeStream& stream, Runtime::GameObjectRef& value)
        {
            if (!TypeIdSerializer<Runtime::GameObjectRef>::Deserialize(stream)) {
                return false;
            }

            value = new Runtime::GameObject();
            Mirror::Any objectRef = std::ref(*value);
            Mirror::Class::Get<Runtime::GameObject>().Deserailize(stream, &objectRef);
            return true;
        }
    };
}

namespace Runtime {
    inline GameObject::GameObject() = default;

    inline GameObject::GameObject(World* inWorld, std::string inName)
        : world(inWorld)
        , name(std::move(inName))
    {
    }

    inline GameObject::~GameObject() = default;

    inline World* GameObject::GetWorld() const
    {
        return world;
    }

    inline void GameObject::SetWorld(World* inWorld)
    {
        world = inWorld;
    }

    inline void GameObject::OnCreated() {}

    inline void GameObject::OnLoaded() {}

    inline void GameObject::OnDestroy() {}

    inline void GameObject::OnStart() {}

    inline void GameObject::OnTick(float frameTimeMs) {}

    inline void GameObject::OnStop() {}
}

#define DeclareGameObjectComponentScripts(componentName) \
    class EClass(scriptDelegate=GameObject) GameObjectComponentScripts_##componentName { \
    public: \
        explicit GameObjectComponentScripts_##componentName(GameObject& inObject); \
    \
        EFunc() \
        componentName& Emplace(componentName&& comp) const \
        { \
            return gameObject.Emplace<componentName>(std::move(comp)); \
        } \
    \
        EFunc() \
        bool Has() const \
        { \
            return gameObject.Has<componentName>(); \
        } \
    \
        EFunc() \
        void Remove() const \
        { \
            gameObject.Remove<componentName>(); \
        } \
    \
    private: \
        GameObject& gameObject; \
    }; \
