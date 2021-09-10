//
// Created by johnk on 2021/9/10.
//

#ifndef EXPLOSION_FACTORY_H
#define EXPLOSION_FACTORY_H

#include <type_traits>
#include <memory>

#include <Mirror/Type.h>

namespace Explosion::Mirror {
    class GlobalFactory {
    public:
        GlobalFactory() = default;
        GlobalFactory(const GlobalFactory&) = delete;
        GlobalFactory& operator=(const GlobalFactory&) = delete;
        ~GlobalFactory() = default;

    private:
    };

    template <typename S>
    class StructFactory {
    public:
        StructFactory() = default;
        StructFactory(const StructFactory<S>&) = delete;
        StructFactory& operator=(const StructFactory<S>&) = delete;
        ~StructFactory() = default;

    private:
    };

    template <typename C>
    class ClassFactory {
    public:
        ClassFactory() = default;
        ClassFactory(const ClassFactory<C>&) = delete;
        ClassFactory& operator=(const ClassFactory<C>&) = delete;
        ~ClassFactory() = default;

    private:
    };

    template <typename T>
    std::enable_if_t<std::is_void_v<T>, GlobalFactory&> Factory()
    {
        static GlobalFactory factory;
        return factory;
    }

    template <typename T>
    std::enable_if_t<std::is_trivially_copyable_v<T>, StructFactory<T>&> Factory()
    {
        static StructFactory<T> factory;
        return factory;
    }

    template <typename T>
    std::enable_if_t<std::is_class_v<T>, ClassFactory<T>&> Factory()
    {
        static ClassFactory<T> factory;
        return factory;
    }
}

#endif//EXPLOSION_FACTORY_H
