//
// Created by johnk on 2025/3/21.
//

#pragma once

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

    class RUNTIME_API EClass() MaterialGraph {
        EClassBody(MaterialGraph)

    public:
        MaterialGraph();

    private:
        // TODO nodes etc.
    };

    struct RUNTIME_API EClass() Material final : Asset {
        EPolyClassBody(Material)

        explicit Material(Core::Uri inUri);
        ~Material() override;

        MaterialType type;
        std::string sourceCode;
#if BUILD_EDITOR
        MaterialGraph graph;
#endif
    };
}
