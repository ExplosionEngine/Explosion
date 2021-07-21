//
// Created by LiZhen on 2021/7/21.
//

#ifndef EXPLOSION_JSONSERIALIZATION_H
#define EXPLOSION_JSONSERIALIZATION_H

#include <string>
#include <Reflection/Reflection.h>

namespace Explosion {

    class JsonSerialization {
    public:
        JsonSerialization() = default;
        ~JsonSerialization() = default;

        static void ToJson(const std::string& path, const ReflAny& any);

        static ReflAny FromJson(const std::string& path);

    };


}

#endif // EXPLOSION_JSONSERIALIZATION_H
