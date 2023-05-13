//
// Created by johnk on 2022/1/24.
//

#include <vector>
#include <functional>

#include <directx/d3dx12.h>

#include <RHI/Common.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/BufferView.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    static D3D12_HEAP_TYPE GetDX12HeapType(BufferUsageFlags bufferUsages)
    {
        static std::unordered_map<BufferUsageFlags, D3D12_HEAP_TYPE> rules = {
            { BufferUsageBits::mapWrite | BufferUsageBits::copySrc, D3D12_HEAP_TYPE_UPLOAD },
            { BufferUsageBits::mapRead | BufferUsageBits::copyDst, D3D12_HEAP_TYPE_READBACK }
            // TODO check other conditions ?
        };
        static D3D12_HEAP_TYPE fallback = D3D12_HEAP_TYPE_DEFAULT;

        for (const auto& rule : rules) {
            if (bufferUsages & rule.first) {
                return rule.second;
            }
        }
        return fallback;
    }

    static D3D12_RESOURCE_STATES GetDX12ResourceStates(BufferUsageFlags bufferUsages)
    {
        static std::unordered_map<BufferUsageBits, D3D12_RESOURCE_STATES> rules = {
            { BufferUsageBits::copySrc, D3D12_RESOURCE_STATE_COPY_SOURCE },
            { BufferUsageBits::copyDst, D3D12_RESOURCE_STATE_COPY_DEST },
            { BufferUsageBits::index, D3D12_RESOURCE_STATE_GENERIC_READ },
            { BufferUsageBits::vertex, D3D12_RESOURCE_STATE_GENERIC_READ },
            { BufferUsageBits::uniform, D3D12_RESOURCE_STATE_GENERIC_READ },
            { BufferUsageBits::storage, D3D12_RESOURCE_STATE_UNORDERED_ACCESS },
            { BufferUsageBits::indirect, D3D12_RESOURCE_STATE_GENERIC_READ },
            // TODO check other conditions ?
        };

        D3D12_RESOURCE_STATES result = D3D12_RESOURCE_STATE_COMMON;
        for (const auto& rule : rules) {
            if (bufferUsages & rule.first) {
                result |= rule.second;
            }
        }
        return result;
    }

    static MapMode GetMapMode(BufferUsageFlags bufferUsages)
    {
        static std::unordered_map<BufferUsageBits, MapMode> rules = {
            { BufferUsageBits::mapRead, MapMode::read },
            { BufferUsageBits::mapWrite, MapMode::write }
        };

        for (const auto& rule : rules) {
            if (bufferUsages & rule.first) {
                return rule.second;
            }
        }
        return MapMode::read;
    }
}

namespace RHI::DirectX12 {
    DX12Buffer::DX12Buffer(DX12Device& device, const BufferCreateInfo& createInfo) : Buffer(createInfo), mapMode(GetMapMode(createInfo.usages)), usages(createInfo.usages), device(device)
    {
        CreateDX12Buffer(device, createInfo);
    }

    DX12Buffer::~DX12Buffer() = default;

    void* DX12Buffer::Map(MapMode mode, size_t offset, size_t length)
    {
        Assert(mapMode == mode);

        void* data;
        CD3DX12_RANGE range(offset, offset + length);
        bool success = SUCCEEDED(dx12Resource->Map(0, &range, &data));
        Assert(success);
        return data;
    }

    void DX12Buffer::UnMap()
    {
        dx12Resource->Unmap(0, nullptr);
    }

    BufferView* DX12Buffer::CreateBufferView(const BufferViewCreateInfo& createInfo)
    {
        return new DX12BufferView(*this, createInfo);
    }

    void DX12Buffer::Destroy()
    {
        delete this;
    }

    ComPtr<ID3D12Resource>& DX12Buffer::GetDX12Resource()
    {
        return dx12Resource;
    }

    BufferUsageFlags DX12Buffer::GetUsages()
    {
        return usages;
    }

    DX12Device& DX12Buffer::GetDevice()
    {
        return device;
    }

    void DX12Buffer::CreateDX12Buffer(DX12Device& device, const BufferCreateInfo& createInfo)
    {
        CD3DX12_HEAP_PROPERTIES heapProperties(GetDX12HeapType(createInfo.usages));
        CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(
            createInfo.usages | BufferUsageBits::uniform ?
            GetConstantBufferSize(createInfo.size) :
            createInfo.size
            );

        bool success = SUCCEEDED(device.GetDX12Device()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            GetDX12ResourceStates(createInfo.usages),
            nullptr,
            IID_PPV_ARGS(&dx12Resource)));
        Assert(success);
    }
}
