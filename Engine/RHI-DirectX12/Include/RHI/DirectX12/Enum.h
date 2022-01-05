//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_ENUM_H
#define EXPLOSION_RHI_DX12_ENUM_H

#include <d3d12.h>
#include <d3dcompiler.h>

#include <unordered_map>

#include <RHI/Enum.h>
#include <RHI/DirectX12/Utility.h>

namespace RHI::DirectX12 {
#define BEGIN_ENUM_CONVERTER(FromType, ToType) template <> const std::unordered_map<FromType, ToType> ENUM_MAP<FromType, ToType> = {
#define END_ENUM_CONVERTER() };
#define ENUM_CONVERTER_ITEM(from, to) { from, to },

    template <typename E, typename DXE>
    const std::unordered_map<E, uint64_t> ENUM_MAP;

    BEGIN_ENUM_CONVERTER(QueueFamilyType, D3D12_COMMAND_LIST_TYPE)
        ENUM_CONVERTER_ITEM(QueueFamilyType::GRAPHICS,  D3D12_COMMAND_LIST_TYPE_DIRECT)
        ENUM_CONVERTER_ITEM(QueueFamilyType::SECONDARY, D3D12_COMMAND_LIST_TYPE_BUNDLE)
        ENUM_CONVERTER_ITEM(QueueFamilyType::COMPUTE,   D3D12_COMMAND_LIST_TYPE_COMPUTE)
        ENUM_CONVERTER_ITEM(QueueFamilyType::COPY,      D3D12_COMMAND_LIST_TYPE_COPY)
    END_ENUM_CONVERTER()

    BEGIN_ENUM_CONVERTER(PixelFormat, DXGI_FORMAT)
        ENUM_CONVERTER_ITEM(PixelFormat::R8G8B8A8_UNORM, DXGI_FORMAT_R8G8B8A8_UNORM)
    END_ENUM_CONVERTER()

    BEGIN_ENUM_CONVERTER(DeviceMemoryType, D3D12_HEAP_TYPE)
        ENUM_CONVERTER_ITEM(DeviceMemoryType::DEVICE_LOCAL, D3D12_HEAP_TYPE_DEFAULT)
        ENUM_CONVERTER_ITEM(DeviceMemoryType::HOST_VISIBLE, D3D12_HEAP_TYPE_UPLOAD)
    END_ENUM_CONVERTER()

    template <typename E, typename DXE>
    DXE EnumCast(const E& e)
    {
        auto iter = ENUM_MAP<E, DXE>.find(e);
        if (iter == ENUM_MAP<E, DXE>.end()) {
            throw DX12Exception("found no suitable enum");
        }
        return iter->second;
    }
}

namespace RHI::DirectX12 {
#define BEGIN_BITS_CONVERTER(Type) template <> const std::unordered_map<Type, UINT> BITS_MAP<Type> = {
#define END_BITS_CONVERTER };
#define BITS_CONVERTER_ITEM(from, to) { from, to },

    template <typename E>
    const std::unordered_map<E, UINT> BITS_MAP;

    BEGIN_BITS_CONVERTER(ShaderCompileBits)
        BITS_CONVERTER_ITEM(ShaderCompileBits::DEBUG, D3DCOMPILE_DEBUG)
        BITS_CONVERTER_ITEM(ShaderCompileBits::NO_OPT, D3DCOMPILE_SKIP_OPTIMIZATION)
    END_BITS_CONVERTER

    template <typename B>
    UINT FlagsCast(Flags flags)
    {
        UINT result = 0;
        for (auto iter : BITS_MAP<B>) {
            if (!(flags & static_cast<Flags>(iter.first))) {
                continue;
            }
            result |= iter.second;
        }
        return result;
    }
}

namespace RHI::DirectX12 {
    std::string GetShaderStageTargetString(ShaderStage stage);
}

#endif //EXPLOSION_RHI_DX12_ENUM_H
