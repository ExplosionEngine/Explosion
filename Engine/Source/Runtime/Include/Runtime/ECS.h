//
// Created by johnk on 2024/10/31.
//

#pragma once

#include <unordered_map>

namespace Runtime {
    using Entity = size_t;
    using CompTypeId = size_t;

    class CompPacket {
    public:
        virtual ~CompPacket();

    protected:
        CompPacket();
    };

    template <typename C>
    class TypedCompPacket final : public CompPacket {
    public:
        ~TypedCompPacket() override;

    private:
        friend class ECRegistry;

        using CompIndex = size_t;

        TypedCompPacket();

        std::unordered_map<Entity, CompIndex> entityMap;
        std::vector<C> storage;
    };

    class EntityContainer {
    public:

    private:
    };

    class ECRegistry {
    public:

    private:
        EntityContainer entities;
        std::unordered_map<CompTypeId, Common::UniqueRef<CompPacket>> compPackets;
    };

    class SystemRegistry {
    public:

    private:
    };
}
