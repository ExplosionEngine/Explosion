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
        NonCopyable(DX12ComputePipeline)
        DX12ComputePipeline(DX12Device& inDevice, const ComputePipelineCreateInfo& inCreateInfo);
        ~DX12ComputePipeline() override;

        void Destroy() override;

        DX12PipelineLayout& GetPipelineLayout();
        ID3D12PipelineState* GetNative();

    private:
        void SavePipelineLayout(const ComputePipelineCreateInfo& createInfo);
        void CreateNativeComputePipeline(DX12Device& inDevice, const ComputePipelineCreateInfo& inCreateInfo);

        DX12PipelineLayout* pipelineLayout;
        ComPtr<ID3D12PipelineState> nativePipelineState;
    };

    class DX12GraphicsPipeline : public GraphicsPipeline {
    public:
        NonCopyable(DX12GraphicsPipeline)
        DX12GraphicsPipeline(DX12Device& inDevice, const GraphicsPipelineCreateInfo& inCreateInfo);
        ~DX12GraphicsPipeline() override;

        void Destroy() override;

        DX12PipelineLayout& GetPipelineLayout();
        ID3D12PipelineState* GetNative();

    private:
        void SavePipelineLayout(const GraphicsPipelineCreateInfo& inCreateInfo);
        void CreateNativeGraphicsPipeline(DX12Device& inDevice, const GraphicsPipelineCreateInfo& inCreateInfo);

        DX12PipelineLayout* pipelineLayout;
        ComPtr<ID3D12PipelineState> nativePipelineState;
    };
}
