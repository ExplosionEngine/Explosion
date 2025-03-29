//
// Created by johnk on 2025/3/21.
//

#pragma once

#include <unordered_map>

#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>

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

    enum class EEnum() MaterialParameterType : uint8_t {
        tInt,
        tFloat,
        tFVec2,
        tFVec3,
        tFVec4,
        tFMat4x4,
        max
    };

    class RUNTIME_API EClass() MaterialParameter {
        EClassBody(MaterialParameter)

    public:
        MaterialParameter();

        EFunc() MaterialParameterType GetType() const;
        EFunc() int32_t GetInt() const;
        EFunc() float GetFloat() const;
        EFunc() Common::FVec2 GetFVec2() const;
        EFunc() Common::FVec3 GetFVec3() const;
        EFunc() Common::FVec4 GetFVec4() const;
        EFunc() Common::FMat4x4 GetFMat4x4() const;
        EFunc() void SetInt(int32_t inValue);
        EFunc() void SetFloat(float inValue);
        EFunc() void SetFVec2(const Common::FVec2& inValue);
        EFunc() void SetFVec3(const Common::FVec3& inValue);
        EFunc() void SetFVec4(const Common::FVec4& inValue);
        EFunc() void SetFMat4x4(const Common::FMat4x4& inValue);

    private:
        std::variant<std::monostate, int32_t, float, Common::FVec2, Common::FVec3, Common::FVec4, Common::FMat4x4> parameter;
    };

    class RUNTIME_API EClass() IMaterial : public Asset {
        EPolyClassBody(IMaterial)

    public:
        using ParameterMap = std::unordered_map<std::string, MaterialParameter>;

        ~IMaterial() override;

        EFunc() virtual MaterialType GetType() const = 0;
        EFunc() virtual std::string GetSourceCode() const = 0;
        EFunc() virtual void SetParameter(const std::string& inName, const MaterialParameter& inParameter) = 0;
        EFunc() virtual ParameterMap& GetParameters() = 0;
        EFunc() virtual const ParameterMap& GetParameters() const = 0;

    protected:
        explicit IMaterial(Core::Uri inUri);
    };

    class RUNTIME_API EClass() Material final : public IMaterial {
        EPolyClassBody(Material)

    public:
        explicit Material(Core::Uri inUri);
        ~Material() override;

        EFunc() MaterialType GetType() const override;
        EFunc() std::string GetSourceCode() const override;
        EFunc() void SetParameter(const std::string& inName, const MaterialParameter& inParameter) override;
        EFunc() ParameterMap& GetParameters() override;
        EFunc() const ParameterMap& GetParameters() const override;

        EFunc() void SetType(MaterialType inType);
        EFunc() void SetSourceCode(const std::string& inSourceCode);

    private:
        EProperty() MaterialType type;
        EProperty() std::string sourceCode;
        EProperty() ParameterMap parameters;
    };

    class RUNTIME_API EClass() MaterialInstance final : public IMaterial {
        EPolyClassBody(MaterialInstance)

    public:
        explicit MaterialInstance(Core::Uri inUri);
        ~MaterialInstance() override;

        EFunc() MaterialType GetType() const override;
        EFunc() std::string GetSourceCode() const override;
        EFunc() void SetParameter(const std::string& inName, const MaterialParameter& inParameter) override;
        EFunc() ParameterMap& GetParameters() override;
        EFunc() const ParameterMap& GetParameters() const override;

        EFunc() AssetPtr<Material> GetMaterial() const;
        EFunc() void SetMaterial(const AssetPtr<Material>& inMaterial);

    private:
        EProperty() AssetPtr<Material> material;
        EProperty() ParameterMap parameters;
    };
}
