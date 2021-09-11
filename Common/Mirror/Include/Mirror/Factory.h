//
// Created by johnk on 2021/9/10.
//

#ifndef EXPLOSION_FACTORY_H
#define EXPLOSION_FACTORY_H

#include <string>
#include <memory>

#include <Mirror/Type.h>

namespace Explosion::Mirror {
    class GlobalFactory {
    public:
        GlobalFactory() = default;
        GlobalFactory(const GlobalFactory&) = delete;
        GlobalFactory& operator=(const GlobalFactory&) = delete;
        ~GlobalFactory() = default;

        template <typename Value>
        GlobalFactory& Variable(const std::string&, Value* address);

        template <typename Ret, typename... Args>
        GlobalFactory& Function(const std::string&, Ret(*func)(Args...));

    private:

    };

    template <typename S>
    class StructFactory {
    public:
        StructFactory() = default;
        StructFactory(const StructFactory<S>&) = delete;
        StructFactory& operator=(const StructFactory<S>&) = delete;
        virtual ~StructFactory() = default;

        template <auto T>
        StructFactory<S>& Variable(const std::string& name);

    private:
        std::unique_ptr<std::string, std::unique_ptr<Internal::VariableInfo>> variables;
    };

    template <typename C>
    class ClassFactory : public StructFactory<C> {
    public:
        ClassFactory() = default;
        ClassFactory(const ClassFactory<C>&) = delete;
        ClassFactory& operator=(const ClassFactory<C>&) = delete;
        ~ClassFactory() override = default;

        template <auto T>
        ClassFactory<C>& Function(const std::string& name);

    private:
        std::unique_ptr<std::string, std::unique_ptr<Internal::FunctionInfo>> functions;
    };

    GlobalFactory& Global()
    {
        static GlobalFactory instance;
        return instance;
    }

    template <typename S>
    StructFactory<S> Struct()
    {
        static StructFactory<S> instance;
        return instance;
    }

    template <typename C>
    ClassFactory<C> Class()
    {
        static ClassFactory<C> instance;
        return instance;
    }
}

#endif//EXPLOSION_FACTORY_H
