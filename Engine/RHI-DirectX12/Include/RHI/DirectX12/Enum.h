//
// Created by johnk on 1/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_ENUM_H
#define EXPLOSION_RHI_DX12_ENUM_H

#include <unordered_map>

#include <RHI/Enum.h>
#include <RHI/DirectX12/Utility.h>

namespace RHI::DirectX12 {
    template <typename E, typename DXE>
    const std::unordered_map<E, uint64_t> ENUM_MAP;

    template <>
    const std::unordered_map<QueueFamilyType, D3D12_COMMAND_LIST_TYPE> ENUM_MAP<QueueFamilyType, D3D12_COMMAND_LIST_TYPE> = {
        { QueueFamilyType::GRAPHICS,  D3D12_COMMAND_LIST_TYPE_DIRECT },
        { QueueFamilyType::SECONDARY, D3D12_COMMAND_LIST_TYPE_BUNDLE },
        { QueueFamilyType::COMPUTE,   D3D12_COMMAND_LIST_TYPE_COMPUTE },
        { QueueFamilyType::COPY,      D3D12_COMMAND_LIST_TYPE_COPY }
    };

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

#endif //EXPLOSION_RHI_DX12_ENUM_H
