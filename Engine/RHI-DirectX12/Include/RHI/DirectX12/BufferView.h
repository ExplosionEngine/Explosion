//
// Created by johnk on 20/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_BUFFER_VIEW_H
#define EXPLOSION_RHI_DX12_BUFFER_VIEW_H

#include <wrl/client.h>
#include <directx/d3dx12.h>
using namespace Microsoft::WRL;

#include <RHI/BufferView.h>

namespace RHI::DirectX12 {
    class DX12Buffer;
    class DX12Device;

    class DX12BufferView : public BufferView {
    public:
        NON_COPYABLE(DX12BufferView)
        DX12BufferView(DX12Buffer& buffer, const BufferViewCreateInfo* createInfo);
        ~DX12BufferView() override;

        void Destroy() override;

        ID3D12DescriptorHeap* GetDX12DescriptorHeap();
        CD3DX12_CPU_DESCRIPTOR_HANDLE GetDX12CpuDescriptorHandle();

    private:
        void CreateDX12Descriptor(const BufferViewCreateInfo* createInfo);

        DX12Buffer& buffer;
        ID3D12DescriptorHeap* dx12DescriptorHeap;
        CD3DX12_CPU_DESCRIPTOR_HANDLE dx12CpuDescriptorHandle;
    };
}

#endif//EXPLOSION_RHI_DX12_BUFFER_VIEW_H
