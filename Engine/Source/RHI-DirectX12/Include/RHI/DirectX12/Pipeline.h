//
// Created by johnk on 11/3/2022.
//

#pragma once

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/Pipeline.h>

using Microsoft::WRL::ComPtr;

namespace RHI::DirectX12 {
    class DX12Device;
    class DX12PipelineLayout;

    class DX12ComputePipeline : public ComputePipeline {
    public:
        NON_COPYABLE(DX12ComputePipeline)
        DX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo);
        ~DX12ComputePipeline() override;

        void Destroy() override;

        DX12PipelineLayout& GetPipelineLayout();
        ComPtr<ID3D12PipelineState>& GetDX12PipelineState();

    private:
        void SavePipelineLayout(const ComputePipelineCreateInfo* createInfo);
        void CreateDX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo);

        DX12PipelineLayout* pipelineLayout;
        ComPtr<ID3D12PipelineState> dx12PipelineState;
    };

    class DX12GraphicsPipeline : public GraphicsPipeline {
    public:
        NON_COPYABLE(DX12GraphicsPipeline)
        DX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo);
        ~DX12GraphicsPipeline() override;

        void Destroy() override;

        DX12PipelineLayout& GetPipelineLayout();
        ComPtr<ID3D12PipelineState>& GetDX12PipelineState();

    private:
        void SavePipelineLayout(const GraphicsPipelineCreateInfo* createInfo);
        void CreateDX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo);

        DX12PipelineLayout* pipelineLayout;
        ComPtr<ID3D12PipelineState> dx12PipelineState;
    };
}
