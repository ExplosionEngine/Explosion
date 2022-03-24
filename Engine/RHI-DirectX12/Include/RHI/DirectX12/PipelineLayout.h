//
// Created by johnk on 11/3/2022.
//

#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/PipelineLayout.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12PipelineLayout : public PipelineLayout {
    public:
        NON_COPYABLE(DX12PipelineLayout)
        DX12PipelineLayout(DX12Device& device, const PipelineLayoutCreateInfo* createInfo);
        ~DX12PipelineLayout() override;

        void Destroy() override;

        ComPtr<ID3D12RootSignature>& GetDX12RootSignature();

    private:
        void CreateDX12RootSignature(DX12Device& device, const PipelineLayoutCreateInfo* createInfo);

        ComPtr<ID3D12RootSignature> dx12RootSignature;
    };
}
