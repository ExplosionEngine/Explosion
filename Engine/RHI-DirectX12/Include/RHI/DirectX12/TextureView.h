//
// Created by johnk on 2022/2/25.
//

#pragma once

#include <memory>

#include <wrl/client.h>
#include <directx/d3dx12.h>
using namespace Microsoft::WRL;

#include <RHI/TextureView.h>

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12Texture;

    class DX12TextureView : public TextureView {
    public:
        NON_COPYABLE(DX12TextureView)
        explicit DX12TextureView(DX12Device& device, DX12Texture& texture, const TextureViewCreateInfo* createInfo);
        ~DX12TextureView() override;

        void Destroy() override;

        ID3D12DescriptorHeap* GetDX12DescriptorHeap();
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDX12CpuDescriptorHandle();
        CD3DX12_GPU_DESCRIPTOR_HANDLE GetDX12GpuDescriptorHandle();

    private:
        void CreateDX12Descriptor(DX12Device& device, const TextureViewCreateInfo* createInfo);

        DX12Texture& texture;
        ID3D12DescriptorHeap* dx12DescriptorHeap;
        CD3DX12_CPU_DESCRIPTOR_HANDLE dx12CpuDescriptorHandle;
        CD3DX12_GPU_DESCRIPTOR_HANDLE dx12GpuDescriptorHandle;
    };
}
