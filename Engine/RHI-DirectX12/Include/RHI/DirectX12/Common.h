//
// Created by johnk on 10/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_COMMON_H
#define EXPLOSION_RHI_DX12_COMMON_H

#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <format>

#include <dxgi1_4.h>
#include <d3d12.h>

#include <RHI/Enum.h>

namespace RHI::DirectX12 {
    class DX12Exception : public std::exception {
    public:
        explicit DX12Exception(std::string msg) : message(std::move(msg)) {}

        [[nodiscard]] const char* what() const override
        {
            return message.c_str();
        }

    private:
        std::string message;
    };
}

// duplicated code because static variable and namespace
namespace RHI::DirectX12 {
    template <typename A, typename B>
    static const std::unordered_map<A, B> DX12_ENUM_MAP;

    template <typename A, typename B>
    B DX12EnumCast(const A& value)
    {
        auto iter = DX12_ENUM_MAP<A, B>.find(value);
        if (iter == DX12_ENUM_MAP<A, B>.end()) {
            throw DX12Exception(std::format("failed to find suitable enum cast result for {}", typeid(A).name()));
        }
        return static_cast<B>(iter->second);
    }

#define DX12_ENUM_MAP_BEGIN(A, B) template <> static const std::unordered_map<A, B> DX12_ENUM_MAP<A, B> = {
#define DX12_ENUM_MAP_ITEM(A, B) { A, B },
#define DX12_ENUM_MAP_END() };
}

// hard code convert
namespace RHI::DirectX12 {
    GpuType GetGpuTypeByAdapterFlag(UINT flag);
}

// enum cast impl
namespace RHI::DirectX12 {
    DX12_ENUM_MAP_BEGIN(QueueType, D3D12_COMMAND_LIST_TYPE)
        DX12_ENUM_MAP_ITEM(QueueType::GRAPHICS, D3D12_COMMAND_LIST_TYPE_BUNDLE)
        DX12_ENUM_MAP_ITEM(QueueType::COMPUTE, D3D12_COMMAND_LIST_TYPE_COMPUTE)
        DX12_ENUM_MAP_ITEM(QueueType::TRANSFER, D3D12_COMMAND_LIST_TYPE_COPY)
    DX12_ENUM_MAP_END()
}

#endif //EXPLOSION_RHI_DX12_COMMON_H
