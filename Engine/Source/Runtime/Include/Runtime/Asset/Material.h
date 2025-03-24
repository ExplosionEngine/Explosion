//
// Created by johnk on 2025/3/21.
//

#pragma once

#include <Common/Math/Vector.h>

#include <Runtime/Asset/Asset.h>
#include <Runtime/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    enum class EEnum() MaterialType : uint8_t {
        surface,
        volume,
        postProcess,
        max
    };

    // TODO material parameter

    class RUNTIME_API EClass() Material : public Asset {
        EPolyClassBody(Material)

    public:
        explicit Material(Core::Uri inUri);
        ~Material() override;

    private:
        EProperty() MaterialType type;
        EProperty() std::string sourceCode;
    };

    class RUNTIME_API EClass() MaterialInstance final : public Material {
        EPolyClassBody(MaterialInstance)

    public:
        explicit MaterialInstance(Core::Uri inUri);
        ~MaterialInstance() override;
    };
}
