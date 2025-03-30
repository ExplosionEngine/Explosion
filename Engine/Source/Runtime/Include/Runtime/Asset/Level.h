//
// Created by johnk on 2025/2/19.
//

#pragma once

#include <Runtime/Meta.h>
#include <Runtime/ECS.h>
#include <Runtime/Asset/Asset.h>
#include <Runtime/Api.h>

namespace Runtime {
    class RUNTIME_API EClass() Level final : public Asset {
        EPolyClassBody(Level)

    public:
        explicit Level(Core::Uri inUri);
        ~Level() override;

        EFunc() ECArchive& GetArchive();

    private:
        EProperty() ECArchive archive;
    };
}
