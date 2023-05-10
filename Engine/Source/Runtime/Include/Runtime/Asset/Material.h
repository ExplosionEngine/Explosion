//
// Created by johnk on 2023/4/5.
//

#pragma once

#include <cstdint>

#include <Runtime/Asset.h>
#include <Runtime/Asset/Texture.h>

#include <Common/Math/Vector.h>

namespace Runtime {
    enum class EEnum() MaterialParameterType {
        FLOAT,
        FLOAT2,
        FLOAT3,
        FLOAT4,
        TEXTURE,
        MAX
    };

    class EClass() MaterialParameterSet {
    public:
        EProperty()
        std::unordered_map<std::string, std::pair<MaterialParameterType, size_t>> parameterTypeAndIndexMap;

        EProperty()
        std::vector<float> floatValues;

        EProperty()
        std::vector<Common::FVec2> float2Values;

        EProperty()
        std::vector<Common::FVec3> float3Values;

        EProperty()
        std::vector<Common::FVec4> float4Values;

        EProperty()
        std::vector<AssetRef<Texture>> textureValues;
    };

    class EClass() MaterialPermutationSet {
    public:
        EProperty()
        std::unordered_map<std::string, uint8_t> valueMap;

        EProperty()
        std::unordered_map<std::string, std::pair<uint8_t, uint8_t>> rangeMap;
    };

    class EClass() Material : public Asset {
    public:
        EProperty()
        std::string code;

        EProperty()
        MaterialParameterSet parameterSet;

        EProperty()
        MaterialPermutationSet permutationSet;
    };

    class EClass() MaterialInstance : public Material {};
}
