//
// Created by johnk on 11/3/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/ShaderModule.h>
#include <RHI/DirectX12/PipelineLayout.h>
#include <RHI/DirectX12/Pipeline.h>

namespace RHI::DirectX12 {
    DX12ComputePipeline::DX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo) : ComputePipeline(createInfo)
    {
        CreateDX12ComputePipeline(device, createInfo);
    }

    DX12ComputePipeline::~DX12ComputePipeline() = default;

    void DX12ComputePipeline::Destroy()
    {
        delete this;
    }

    ComPtr<ID3D12PipelineState>& DX12ComputePipeline::GetDX12PipelineState()
    {
        return dx12PipelineState;
    }

    void DX12ComputePipeline::CreateDX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo)
    {
        auto* pipelineLayout = dynamic_cast<DX12PipelineLayout*>(createInfo->layout);
        auto* computeShader = dynamic_cast<DX12ShaderModule*>(createInfo->computeShader);

        D3D12_COMPUTE_PIPELINE_STATE_DESC desc {};
        desc.pRootSignature = pipelineLayout->GetDX12RootSignature().Get();
        desc.CS = computeShader->GetDX12ShaderBytecode();

        if (FAILED(device.GetDX12Device()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&dx12PipelineState)))) {
            throw DX12Exception("failed to create dx12 compute pipeline state");
        }
    }

    DX12GraphicsPipeline::DX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo) : GraphicsPipeline(createInfo)
    {
        CreateDX12GraphicsPipeline(device, createInfo);
    }

    DX12GraphicsPipeline::~DX12GraphicsPipeline() = default;

    void DX12GraphicsPipeline::Destroy()
    {
        delete this;
    }

    ComPtr<ID3D12PipelineState>& DX12GraphicsPipeline::GetDX12PipelineState()
    {
        return dx12PipelineState;
    }

    void DX12GraphicsPipeline::CreateDX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo)
    {
        // TODO
    }
}
