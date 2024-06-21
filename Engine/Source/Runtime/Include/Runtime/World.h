//
// Created by johnk on 2023/9/5.
//

#pragma once

#include <Runtime/ECS.h>
#include <Runtime/Asset/Level.h>

namespace Runtime {
    class World;

    class RUNTIME_API World : public ECSHost {
    public:
        explicit World(std::string inName = "");
        ~World();

        void Setup() override;
        void Tick(float timeMS) override;
        void Shutdown() override;
        bool Setuped() override;
        void Reset() override;

        void LoadFromLevel(const AssetRef<Level>& level);
        void SaveToLevel(AssetRef<Level>& level) const;

    private:
        std::string name;
    };
}
