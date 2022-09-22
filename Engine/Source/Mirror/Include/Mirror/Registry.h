//
// Created by johnk on 2022/9/10.
//

#pragma once

#include <unordered_map>

#include <Mirror/Api.h>
#include <Mirror/Type.h>

namespace Mirror {
    class MIRROR_API ClassRegistry {
    public:
        ~ClassRegistry() = default;

    private:
        explicit ClassRegistry(Class& inClass) : clazz(inClass) {}

        Class& clazz;
    };

    class MIRROR_API GlobalRegistry {
    public:
        ~GlobalRegistry() = default;

    private:
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

        GlobalScope& Global()
        {
            return globalScope;
        }

        // TODO

    private:
        GlobalScope globalScope;
        std::unordered_map<std::string, Class> classes;
    };
}
