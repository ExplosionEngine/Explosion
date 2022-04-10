//
// Created by johnk on 11/3/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/ShaderModule.h>
#include <RHI/DirectX12/PipelineLayout.h>
#include <RHI/DirectX12/Pipeline.h>

namespace RHI::DirectX12 {
    D3D12_RENDER_TARGET_BLEND_DESC GetDX12RenderTargetBlendDesc(const BlendState& blendState)
    {
        D3D12_RENDER_TARGET_BLEND_DESC desc {};
        desc.BlendEnable = true;
        desc.LogicOpEnable = false;
        desc.BlendOp = DX12EnumCast<BlendOp, D3D12_BLEND_OP>(blendState.color.op);
        desc.SrcBlend = DX12EnumCast<BlendFactor, D3D12_BLEND>(blendState.color.srcFactor);
        desc.DestBlend = DX12EnumCast<BlendFactor, D3D12_BLEND>(blendState.color.dstFactor);
        desc.BlendOpAlpha = DX12EnumCast<BlendOp, D3D12_BLEND_OP>(blendState.alpha.op);
        desc.SrcBlendAlpha = DX12EnumCast<BlendFactor, D3D12_BLEND>(blendState.alpha.srcFactor);
        desc.DestBlendAlpha = DX12EnumCast<BlendFactor, D3D12_BLEND>(blendState.alpha.dstFactor);
        return desc;
    }

    CD3DX12_RASTERIZER_DESC GetDX12RasterizerDesc(const GraphicsPipelineCreateInfo* createInfo)
    {
        CD3DX12_RASTERIZER_DESC desc(D3D12_DEFAULT);
        // TODO expose to RHI interface?
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = DX12EnumCast<CullMode, D3D12_CULL_MODE>(createInfo->primitive.cullMode);
        desc.FrontCounterClockwise = createInfo->primitive.frontFace == FrontFace::CCW;
        desc.DepthBias = createInfo->depthStencil.depthBias;
        desc.DepthBiasClamp = createInfo->depthStencil.depthBiasClamp;
        desc.SlopeScaledDepthBias = createInfo->depthStencil.depthBiasSlopeScale;
        desc.DepthClipEnable = createInfo->primitive.depthClip;
        // TODO check this
        desc.MultisampleEnable = createInfo->multiSample.count > 1;
        desc.AntialiasedLineEnable = false;
        desc.ForcedSampleCount = 0;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        return desc;
    }

    CD3DX12_BLEND_DESC GetDX12BlendDesc(const GraphicsPipelineCreateInfo* createInfo)
    {
        CD3DX12_BLEND_DESC desc(D3D12_DEFAULT);
        desc.AlphaToCoverageEnable = createInfo->multiSample.alphaToCoverage;
        desc.IndependentBlendEnable = true;

        Assert(createInfo->fragment.colorTargetNum <= 8);
        for (auto i = 0; i < createInfo->fragment.colorTargetNum; i++) {
            desc.RenderTarget[i] = GetDX12RenderTargetBlendDesc(createInfo->fragment.colorTargets[i].blend);
        }
        return desc;
    }

    D3D12_DEPTH_STENCILOP_DESC GetDX12DepthStencilOpDesc(const StencilFaceState& stencilFaceState)
    {
        D3D12_DEPTH_STENCILOP_DESC desc {};
        desc.StencilFailOp = DX12EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.failOp);
        desc.StencilDepthFailOp = DX12EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.depthFailOp);
        desc.StencilPassOp = DX12EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.passOp);
        desc.StencilFunc = DX12EnumCast<ComparisonFunc, D3D12_COMPARISON_FUNC>(stencilFaceState.comparisonFunc);
        return desc;
    }

    CD3DX12_DEPTH_STENCIL_DESC GetDX12DepthStencilDesc(const GraphicsPipelineCreateInfo* createInfo)
    {
        CD3DX12_DEPTH_STENCIL_DESC desc(D3D12_DEFAULT);
        // TODO check this
        desc.DepthEnable = createInfo->depthStencil.depthEnable;
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = DX12EnumCast<ComparisonFunc, D3D12_COMPARISON_FUNC>(createInfo->depthStencil.depthComparisonFunc);
        desc.StencilEnable = createInfo->depthStencil.stencilEnable;
        desc.StencilReadMask = createInfo->depthStencil.stencilReadMask;
        desc.StencilWriteMask = createInfo->depthStencil.stencilWriteMask;
        desc.FrontFace = GetDX12DepthStencilOpDesc(createInfo->depthStencil.stencilFront);
        desc.BackFace = GetDX12DepthStencilOpDesc(createInfo->depthStencil.stencilBack);
        return desc;
    }

    DXGI_SAMPLE_DESC GetDX12SampleDesc(const GraphicsPipelineCreateInfo* createInfo)
    {
        DXGI_SAMPLE_DESC desc {};
        desc.Count = createInfo->multiSample.count;
        // TODO https://docs.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
        desc.Quality = 0;
        return desc;
    }

    UINT GetDX12SampleMask(const GraphicsPipelineCreateInfo* createInfo)
    {
        return createInfo->multiSample.mask;
    }

    void UpdateDX12RenderTargetsDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const GraphicsPipelineCreateInfo* createInfo)
    {
        // have been checked num in function #GetDX12BlendDesc()
        desc.NumRenderTargets = createInfo->fragment.colorTargetNum;
        for (auto i = 0; i < createInfo->fragment.colorTargetNum; i++) {
            desc.RTVFormats[i] = DX12EnumCast<PixelFormat, DXGI_FORMAT>(createInfo->fragment.colorTargets[i].format);
        }
    }
}

namespace RHI::DirectX12 {
    DX12ComputePipeline::DX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo) : ComputePipeline(createInfo)
    {
        SavePipelineLayout(createInfo);
        CreateDX12ComputePipeline(device, createInfo);
    }

    DX12ComputePipeline::~DX12ComputePipeline() = default;

    void DX12ComputePipeline::Destroy()
    {
        delete this;
    }

    DX12PipelineLayout& DX12ComputePipeline::GetPipelineLayout()
    {
        return *pipelineLayout;
    }

    ComPtr<ID3D12PipelineState>& DX12ComputePipeline::GetDX12PipelineState()
    {
        return dx12PipelineState;
    }

    void DX12ComputePipeline::SavePipelineLayout(const ComputePipelineCreateInfo* createInfo)
    {
        auto* pl = dynamic_cast<DX12PipelineLayout*>(createInfo->layout);
        Assert(pl);
        pipelineLayout = pl;
    }

    void DX12ComputePipeline::CreateDX12ComputePipeline(DX12Device& device, const ComputePipelineCreateInfo* createInfo)
    {
        auto* computeShader = dynamic_cast<DX12ShaderModule*>(createInfo->computeShader);

        D3D12_COMPUTE_PIPELINE_STATE_DESC desc {};
        desc.pRootSignature = pipelineLayout->GetDX12RootSignature().Get();
        desc.CS = computeShader->GetDX12ShaderBytecode();

        bool success = SUCCEEDED(device.GetDX12Device()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&dx12PipelineState)));
        Assert(success);
    }

    DX12GraphicsPipeline::DX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo) : GraphicsPipeline(createInfo)
    {
        SavePipelineLayout(createInfo);
        CreateDX12GraphicsPipeline(device, createInfo);
    }

    DX12GraphicsPipeline::~DX12GraphicsPipeline() = default;

    void DX12GraphicsPipeline::Destroy()
    {
        delete this;
    }

    DX12PipelineLayout& DX12GraphicsPipeline::GetPipelineLayout()
    {
        return *pipelineLayout;
    }

    ComPtr<ID3D12PipelineState>& DX12GraphicsPipeline::GetDX12PipelineState()
    {
        return dx12PipelineState;
    }

    void DX12GraphicsPipeline::SavePipelineLayout(const GraphicsPipelineCreateInfo* createInfo)
    {
        auto* pl = dynamic_cast<DX12PipelineLayout*>(createInfo->layout);
        Assert(pl);
        pipelineLayout = pl;
    }

    void DX12GraphicsPipeline::CreateDX12GraphicsPipeline(DX12Device& device, const GraphicsPipelineCreateInfo* createInfo)
    {
        auto* vertexShader = dynamic_cast<DX12ShaderModule*>(createInfo->vertexShader);
        auto* fragmentShader = dynamic_cast<DX12ShaderModule*>(createInfo->fragmentShader);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc {};
        desc.pRootSignature = pipelineLayout->GetDX12RootSignature().Get();
        desc.VS = vertexShader->GetDX12ShaderBytecode();
        desc.PS = fragmentShader->GetDX12ShaderBytecode();
        desc.RasterizerState = GetDX12RasterizerDesc(createInfo);
        desc.BlendState = GetDX12BlendDesc(createInfo);
        desc.DepthStencilState = GetDX12DepthStencilDesc(createInfo);
        desc.SampleMask = GetDX12SampleMask(createInfo);
        desc.SampleDesc = GetDX12SampleDesc(createInfo);
        desc.PrimitiveTopologyType = DX12EnumCast<PrimitiveTopology, D3D12_PRIMITIVE_TOPOLOGY_TYPE>(createInfo->primitive.topology);
        UpdateDX12RenderTargetsDesc(desc, createInfo);

        bool success = SUCCEEDED(device.GetDX12Device()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&dx12PipelineState)));
        Assert(success);
    }
}
