//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <unordered_map>

#include <Mirror/Api.h>
#include <Mirror/Type.h>

namespace Mirror {
    template <typename E>
    class MIRROR_API MetaDataRegistry {
    public:
        ~MetaDataRegistry() = default;

        MetaDataRegistry Value(const std::string& key, const std::string& value)
        {
            type.metas[key] = value;
        }

        E& End()
        {
            return end;
        }

    private:
        friend E;

        explicit MetaDataRegistry(E& inEnd, Type& inType) : end(inEnd), type(inType) {}

        E& end;
        Type& type;
    };

    class MIRROR_API VariableRegistry {
    public:
        ~VariableRegistry() = default;

        auto MetaData()
        {
            return MetaDataRegistry<VariableRegistry>(*this, variable);
        }

        // TODO

    private:
        friend class GlobalRegistry;

        explicit VariableRegistry(Variable& inVariable) : variable(inVariable) {}

        Variable& variable;
    };

    template <typename C>
    class MIRROR_API ClassRegistry {
    public:
        ~ClassRegistry() = default;

        auto MetaData()
        {
            return MetaDataRegistry<ClassRegistry<C>>(*this, clazz);
        }

        // TODO

    private:
        friend class Registry;

        explicit ClassRegistry(Class& inClass) : clazz(inClass) {}

        Class& clazz;
    };

    class MIRROR_API GlobalRegistry {
    public:
        ~GlobalRegistry() = default;

        // TODO

    private:
        friend class Registry;

        explicit GlobalRegistry(GlobalScope& inGlobalScope) : globalScope(inGlobalScope) {}

        GlobalScope& globalScope;
    };

    class MIRROR_API Registry {
    public:
        static Registry Get()
        {
            static Registry instance;
            return instance;
        }

        ~Registry() = default;

        GlobalRegistry Global()
        {
            return GlobalRegistry(globalScope);
        }

        template <typename C>
        ClassRegistry<C> Class(const std::string& name)
        {
            classes[name] = Mirror::Class(name);
            return ClassRegistry<C>(classes[name]);
        }

    private:
        Registry() noexcept = default;

        GlobalScope globalScope;
        std::unordered_map<std::string, Mirror::Class> classes;
    };
}
