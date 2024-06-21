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
        };
        static D3D12_HEAP_TYPE fallback = D3D12_HEAP_TYPE_DEFAULT;

        for (const auto& rule : rules) {
            if (bufferUsages & rule.first) {
                return rule.second;
            }
        }
        return fallback;
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
    DX12Buffer::DX12Buffer(DX12Device& device, const BufferCreateInfo& inCreateInfo)
        : Buffer(inCreateInfo)
        , device(device)
        , mapMode(GetMapMode(inCreateInfo.usages))
        , usages(inCreateInfo.usages)
    {
        CreateNativeBuffer(device, inCreateInfo);
    }

    DX12Buffer::~DX12Buffer() = default;

    void* DX12Buffer::Map(const MapMode inMapMode, const size_t inOffset, const size_t inLength)
    {
        Assert(mapMode == inMapMode);

        void* data;
        const CD3DX12_RANGE range(inOffset, inOffset + inLength);
        Assert(SUCCEEDED(nativeResource->Map(0, &range, &data)));
        return data;
    }

    void DX12Buffer::UnMap()
    {
        nativeResource->Unmap(0, nullptr);
    }

    Common::UniqueRef<BufferView> DX12Buffer::CreateBufferView(const BufferViewCreateInfo& inCreateInfo)
    {
        return Common::UniqueRef<BufferView>(new DX12BufferView(*this, inCreateInfo));
    }

    ID3D12Resource* DX12Buffer::GetNative() const
    {
        return nativeResource.Get();
    }

    BufferUsageFlags DX12Buffer::GetUsages() const
    {
        return usages;
    }

    DX12Device& DX12Buffer::GetDevice() const
    {
        return device;
    }

    void DX12Buffer::CreateNativeBuffer(DX12Device& inDevice, const BufferCreateInfo& inCreateInfo)
    {
        const CD3DX12_HEAP_PROPERTIES heapProperties(GetDX12HeapType(inCreateInfo.usages));
        const CD3DX12_RESOURCE_DESC resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(
            inCreateInfo.usages & BufferUsageBits::uniform ?
            Common::AlignUp<D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT>(inCreateInfo.size) :
            inCreateInfo.size);

        const bool success = SUCCEEDED(inDevice.GetNative()->CreateCommittedResource(
            &heapProperties,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            EnumCast<BufferState, D3D12_RESOURCE_STATES>(inCreateInfo.initialState),
            nullptr,
            IID_PPV_ARGS(&nativeResource)));
        Assert(success);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            Assert(SUCCEEDED(nativeResource->SetName(Common::StringUtils::ToWideString(inCreateInfo.debugName).c_str())));
        }
#endif
    }
}