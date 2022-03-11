//
// Created by johnk on 11/3/2022.
//

#ifndef EXPLOSION_RHI_DX12_PIPELINE_H
#define EXPLOSION_RHI_DX12_PIPELINE_H

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/Pipeline.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Device;

    class DX12ComputePipeline : public ComputePipeline {
    public:
        NON_COPYABLE(DX12ComputePipeline)
        DX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo);
        ~DX12ComputePipeline() override;

        void Destroy() override;

        ComPtr<ID3D12PipelineState>& GetDX12PipelineState();

    private:
        void CreateDX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo);

        ComPtr<ID3D12PipelineState> dx12PipelineState;
    };

    class DX12GraphicsPipeline : public GraphicsPipeline {
    public:
        NON_COPYABLE(DX12GraphicsPipeline)
        DX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo);
        ~DX12GraphicsPipeline() override;

        void Destroy() override;

        ComPtr<ID3D12PipelineState>& GetDX12PipelineState();

    private:
        void CreateDX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo);

        ComPtr<ID3D12PipelineState> dx12PipelineState;
    };
}

#endif//EXPLOSION_RHI_DX12_PIPELINE_H
