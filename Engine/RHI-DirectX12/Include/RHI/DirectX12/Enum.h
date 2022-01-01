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
    const std::unordered_map<CommandQueueType, D3D12_COMMAND_LIST_TYPE> ENUM_MAP<CommandQueueType, D3D12_COMMAND_LIST_TYPE> = {
        { CommandQueueType::GRAPHICS, D3D12_COMMAND_LIST_TYPE_DIRECT },
        { CommandQueueType::SECONDARY, D3D12_COMMAND_LIST_TYPE_BUNDLE },
        { CommandQueueType::COMPUTE, D3D12_COMMAND_LIST_TYPE_COMPUTE },
        { CommandQueueType::COPY, D3D12_COMMAND_LIST_TYPE_COPY }
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
