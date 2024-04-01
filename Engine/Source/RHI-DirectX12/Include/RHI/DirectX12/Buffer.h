//
// Created by johnk on 2022/1/24.
//

#pragma once

#include <wrl/client.h>
#include <directx/d3dx12.h>

#include <RHI/Buffer.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12Buffer : public Buffer {
    public:
        NonCopyable(DX12Buffer)
        explicit DX12Buffer(DX12Device& device, const BufferCreateInfo& inCreateInfo);
        ~DX12Buffer() override;

        void* Map(MapMode inMapMode, size_t inOffset, size_t inLength) override;
        void UnMap() override;
        BufferView* CreateBufferView(const BufferViewCreateInfo& inCreateInfo) override;
        void Destroy() override;

        ID3D12Resource* GetNative();
        DX12Device& GetDevice();
        BufferUsageFlags GetUsages();

    private:
        void CreateNativeBuffer(DX12Device& inDevice, const BufferCreateInfo& inCreateInfo);

        DX12Device& device;
        MapMode mapMode;
        BufferUsageFlags usages;
        ComPtr<ID3D12Resource> nativeResource;
    };
}
