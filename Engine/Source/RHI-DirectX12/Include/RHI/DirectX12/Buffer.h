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
        explicit DX12Buffer(DX12Device& device, const BufferCreateInfo& createInfo);
        ~DX12Buffer() override;

        void* Map(MapMode mapMode, size_t offset, size_t length) override;
        void UnMap() override;
        BufferView* CreateBufferView(const BufferViewCreateInfo& createInfo) override;
        void Destroy() override;

        ComPtr<ID3D12Resource>& GetDX12Resource();
        DX12Device& GetDevice();
        BufferUsageFlags GetUsages();

    private:
        void CreateDX12Buffer(DX12Device& device, const BufferCreateInfo& createInfo);

        DX12Device& device;
        MapMode mapMode;
        BufferUsageFlags usages;
        ComPtr<ID3D12Resource> dx12Resource;
    };
}
