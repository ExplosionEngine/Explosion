//
// Created by johnk on 2025/3/24.
//

#pragma once

#include <cstdint>

#include <RHI/Common.h>
#include <Runtime/Asset/Asset.h>
#include <Runtime/RenderThreadPtr.h>
#include <Runtime/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    enum class EEnum() TextureType : uint8_t {
        t1D,
        t2D,
        t2DArray,
        tCube,
        tCubeArray,
        t3D,
        max
    };

    enum class EEnum() TextureFormat : uint8_t {
        // 8-Bits
        begin8Bits,
        r8Unorm,
        r8Snorm,
        r8Uint,
        r8Sint,
        // 16-Bits
        begin16Bits,
        r16Uint,
        r16Sint,
        r16Float,
        rg8Unorm,
        rg8Snorm,
        rg8Uint,
        rg8Sint,
        d16Unorm,
        // 32-Bits
        begin32Bits,
        r32Uint,
        r32Sint,
        r32Float,
        rg16Uint,
        rg16Sint,
        rg16Float,
        rgba8Unorm,
        rgba8UnormSrgb,
        rgba8Snorm,
        rgba8Uint,
        rgba8Sint,
        bgra8Unorm,
        bgra8UnormSrgb,
        rgb9E5Float,
        rgb10A2Unorm,
        rg11B10Float,
        d24UnormS8Uint,
        d32Float,
        // 64-Bits
        begin64Bits,
        rg32Uint,
        rg32Sint,
        rg32Float,
        rgba16Uint,
        rgba16Sint,
        rgba16Float,
        d32FloatS8Uint,
        // 128-Bits
        begin128Bits,
        rgba32Uint,
        rgba32Sint,
        rgba32Float,
        max
    };
    static_assert(static_cast<uint8_t>(TextureFormat::max) == static_cast<uint8_t>(RHI::PixelFormat::max));

    class RUNTIME_API EClass() Texture final : public Asset {
        EPolyClassBody(Texture)

    public:
        using Pixels = std::vector<uint8_t>;

        explicit Texture(Core::Uri inUri);
        ~Texture() override;

        void PostLoad() override;

        EFunc() TextureType GetType() const;
        EFunc() TextureFormat GetFormat() const;
        EFunc() uint32_t GetWidth() const;
        EFunc() uint32_t GetHeight() const;
        EFunc() uint32_t GetDepthOrArraySize() const;
        EFunc() uint8_t GetMipLevels() const;
        EFunc() uint8_t GetSamples() const;
        EFunc() const std::string& GetName() const;
        EFunc() Pixels& GetSubResourcePixels(uint8_t inMipLevel, uint8_t inArrayLayer);
        EFunc() const Pixels& GetSubResourcePixels(uint8_t inMipLevel, uint8_t inArrayLayer) const;
        EFunc() void SetType(TextureType inType);
        EFunc() void SetFormat(TextureFormat inFormat);
        EFunc() void SetWidth(uint32_t inWidth);
        EFunc() void SetHeight(uint32_t inHeight);
        EFunc() void SetDepthOrArraySize(uint32_t inDepthOrArraySize);
        EFunc() void SetMipLevels(uint8_t inMipLevels);
        EFunc() void SetSamples(uint8_t inSamples);
        EFunc() void SetName(const std::string& inName);
        EFunc() RHI::Texture* GetRHI() const;
        EFunc() RHI::TextureView* GetViewRHI() const;
        EFunc() void UpdateMips();
        EFunc() void UpdateRHI();

    private:
        EProperty() TextureType type;
        EProperty() TextureFormat format;
        EProperty() uint32_t width;
        EProperty() uint32_t height;
        EProperty() uint32_t depthOrArraySize;
        EProperty() uint8_t mipLevels;
        EProperty() uint8_t samples;
        EProperty() std::string name;
        EProperty() std::vector<Pixels> subResourcePixelsData;
        RenderThreadPtr<RHI::Texture> texture;
        RenderThreadPtr<RHI::TextureView> textureView;
    };

    class RUNTIME_API EClass() RenderTarget final : public Asset {
        EPolyClassBody(RenderTarget)

    public:
        explicit RenderTarget(Core::Uri inUri);
        ~RenderTarget() override;

        void PostLoad() override;

        EFunc() TextureType GetType() const;
        EFunc() TextureFormat GetFormat() const;
        EFunc() uint32_t GetWidth() const;
        EFunc() uint32_t GetHeight() const;
        EFunc() uint32_t GetDepthOrArraySize() const;
        EFunc() uint8_t GetMipLevels() const;
        EFunc() uint8_t GetSamples() const;
        EFunc() const std::string& GetName() const;
        EFunc() void SetType(TextureType inType);
        EFunc() void SetFormat(TextureFormat inFormat);
        EFunc() void SetWidth(uint32_t inWidth);
        EFunc() void SetHeight(uint32_t inHeight);
        EFunc() void SetDepthOrArraySize(uint32_t inDepthOrArraySize);
        EFunc() void SetMipLevels(uint8_t inMipLevels);
        EFunc() void SetSamples(uint8_t inSamples);
        EFunc() void SetName(const std::string& inName);
        EFunc() RHI::Texture* GetRHI() const;
        EFunc() RHI::TextureView* GetRenderTargetViewRHI() const;
        EFunc() RHI::TextureView* GetShaderResourceViewRHI() const;
        EFunc() void UpdateRHI();

    private:
        EProperty() TextureType type;
        EProperty() TextureFormat format;
        EProperty() uint32_t width;
        EProperty() uint32_t height;
        EProperty() uint32_t depthOrArraySize;
        EProperty() uint8_t mipLevels;
        EProperty() uint8_t samples;
        EProperty() std::string name;
        RenderThreadPtr<RHI::Texture> texture;
        RenderThreadPtr<RHI::TextureView> renderTargetView;
        RenderThreadPtr<RHI::TextureView> shaderResourceView;
    };
}
