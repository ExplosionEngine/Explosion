//
// Created by johnk on 2024/6/29.
//

#pragma once

#include <Mirror/Meta.h>
#include <Mirror/Mirror.h>
#include <Common/Serialization.h>

namespace Runtime {
    struct EClass() Component {
        EClassBody(Component)
    };

    struct ComponentStorage {
        const Mirror::Class* clazz;
        Mirror::Any storage;

        ComponentStorage();
        ~ComponentStorage();
        ComponentStorage(ComponentStorage&& inOther) noexcept;
        ComponentStorage(const ComponentStorage& inOther);
        ComponentStorage& operator=(const ComponentStorage& inOther);
    };
}

namespace Common { // NOLINT
    template <>
    struct Serializer<Runtime::ComponentStorage> {
        static constexpr bool serializable = true;
        static constexpr uint32_t typeId = Common::HashUtils::StrCrc32("Runtime::ComponentStorage");

        static void Serialize(SerializeStream& stream, const Runtime::ComponentStorage& value)
        {
            TypeIdSerializer<Runtime::ComponentStorage>::Serialize(stream);

            Serializer<std::string>::Serialize(stream, value.clazz->GetName());
            value.clazz->Serialize(stream, const_cast<Mirror::Any*>(&value.storage));
        }

        static bool Deserialize(DeserializeStream& stream, Runtime::ComponentStorage& value)
        {
            if (!TypeIdSerializer<Runtime::ComponentStorage>::Deserialize(stream)) {
                return false;
            }

            std::string className;
            Serializer<std::string>::Deserialize(stream, className);
            value.clazz = Mirror::Class::Find(className);
            value.storage = value.clazz->GetDefaultConstructor().ConstructOnStack();
            value.clazz->Deserailize(stream, &value.storage);
            return true;
        }
    };
}

namespace Runtime {
    inline ComponentStorage::ComponentStorage() = default;

    inline ComponentStorage::~ComponentStorage() = default;

    inline ComponentStorage::ComponentStorage(ComponentStorage&& inOther) noexcept
        : clazz(inOther.clazz)
        , storage(std::move(inOther.storage))
    {
    }

    inline ComponentStorage::ComponentStorage(const ComponentStorage& inOther) = default;

    inline ComponentStorage& ComponentStorage::operator=(const ComponentStorage& inOther) = default;
}
