 //
// Created by johnk on 11/3/2022.
//

#include <vector>
#include <utility>

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/ShaderModule.h>
#include <RHI/DirectX12/PipelineLayout.h>
#include <RHI/DirectX12/Pipeline.h>

namespace RHI::DirectX12 {
    D3D12_RENDER_TARGET_BLEND_DESC GetDX12RenderTargetBlendDesc(const ColorTargetState& colorTargetState)
    {
        D3D12_RENDER_TARGET_BLEND_DESC desc {};
        desc.BlendEnable = colorTargetState.blendEnabled;
        desc.LogicOpEnable = false;
        desc.RenderTargetWriteMask = FlagsCast<ColorWriteFlags, uint8_t>(colorTargetState.writeFlags);
        desc.BlendOp = EnumCast<BlendOp, D3D12_BLEND_OP>(colorTargetState.colorBlend.op);
        desc.SrcBlend = EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.colorBlend.srcFactor);
        desc.DestBlend = EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.colorBlend.dstFactor);
        desc.BlendOpAlpha = EnumCast<BlendOp, D3D12_BLEND_OP>(colorTargetState.alphaBlend.op);
        desc.SrcBlendAlpha = EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.alphaBlend.srcFactor);
        desc.DestBlendAlpha = EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.alphaBlend.dstFactor);
        return desc;
    }

    CD3DX12_RASTERIZER_DESC GetDX12RasterizerDesc(const RasterPipelineCreateInfo& createInfo)
    {
        CD3DX12_RASTERIZER_DESC desc(D3D12_DEFAULT);
        desc.FillMode = EnumCast<FillMode, D3D12_FILL_MODE>(createInfo.primitiveState.fillMode);
        desc.CullMode = EnumCast<CullMode, D3D12_CULL_MODE>(createInfo.primitiveState.cullMode);
        desc.FrontCounterClockwise = createInfo.primitiveState.frontFace == FrontFace::ccw;
        desc.DepthBias = createInfo.depthStencilState.depthBias;
        desc.DepthBiasClamp = createInfo.depthStencilState.depthBiasClamp;
        desc.SlopeScaledDepthBias = createInfo.depthStencilState.depthBiasSlopeScale;
        desc.DepthClipEnable = createInfo.primitiveState.depthClip;
        desc.MultisampleEnable = createInfo.multiSampleState.count > 1;
        desc.AntialiasedLineEnable = false;
        desc.ForcedSampleCount = 0;
        desc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        return desc;
    }

    CD3DX12_BLEND_DESC GetDX12BlendDesc(const RasterPipelineCreateInfo& createInfo)
    {
        CD3DX12_BLEND_DESC desc(D3D12_DEFAULT);
        desc.AlphaToCoverageEnable = createInfo.multiSampleState.alphaToCoverage;
        desc.IndependentBlendEnable = true;

        Assert(createInfo.fragmentState.colorTargets.size() <= 8);
        for (auto i = 0; i < createInfo.fragmentState.colorTargets.size(); i++) {
            desc.RenderTarget[i] = GetDX12RenderTargetBlendDesc(createInfo.fragmentState.colorTargets[i]);
        }
        return desc;
    }

    D3D12_DEPTH_STENCILOP_DESC GetDX12DepthStencilOpDesc(const StencilFaceState& stencilFaceState)
    {
        D3D12_DEPTH_STENCILOP_DESC desc {};
        desc.StencilFailOp = EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.failOp);
        desc.StencilDepthFailOp = EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.depthFailOp);
        desc.StencilPassOp = EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.passOp);
        desc.StencilFunc = EnumCast<CompareFunc, D3D12_COMPARISON_FUNC>(stencilFaceState.compareFunc);
        return desc;
    }

    CD3DX12_DEPTH_STENCIL_DESC GetDX12DepthStencilDesc(const RasterPipelineCreateInfo& createInfo)
    {
        CD3DX12_DEPTH_STENCIL_DESC desc(D3D12_DEFAULT);
        desc.DepthEnable = createInfo.depthStencilState.depthEnabled;
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = EnumCast<CompareFunc, D3D12_COMPARISON_FUNC>(createInfo.depthStencilState.depthCompareFunc);
        desc.StencilEnable = createInfo.depthStencilState.stencilEnabled;
        desc.StencilReadMask = createInfo.depthStencilState.stencilReadMask;
        desc.StencilWriteMask = createInfo.depthStencilState.stencilWriteMask;
        desc.FrontFace = GetDX12DepthStencilOpDesc(createInfo.depthStencilState.stencilFront);
        desc.BackFace = GetDX12DepthStencilOpDesc(createInfo.depthStencilState.stencilBack);
        return desc;
    }

    DXGI_SAMPLE_DESC GetDX12SampleDesc(const RasterPipelineCreateInfo& createInfo)
    {
        DXGI_SAMPLE_DESC desc {};
        desc.Count = createInfo.multiSampleState.count;
        desc.Quality = 0;
        return desc;
    }

    UINT GetDX12SampleMask(const RasterPipelineCreateInfo& createInfo)
    {
        return createInfo.multiSampleState.mask;
    }

    void UpdateDX12RenderTargetsDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const RasterPipelineCreateInfo& createInfo)
    {
        // have been checked num in function #GetDX12BlendDesc()
        desc.NumRenderTargets = createInfo.fragmentState.colorTargets.size();
        for (auto i = 0; i < createInfo.fragmentState.colorTargets.size(); i++) {
            desc.RTVFormats[i] = EnumCast<PixelFormat, DXGI_FORMAT>(createInfo.fragmentState.colorTargets[i].format);
        }
    }

    void UpdateDX12DepthStencilTargetDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const RasterPipelineCreateInfo& createInfo)
    {
        if (!createInfo.depthStencilState.depthEnabled && !createInfo.depthStencilState.stencilEnabled) {
            return;
        }
        desc.DSVFormat = EnumCast<PixelFormat, DXGI_FORMAT>(createInfo.depthStencilState.format);
    }

    std::vector<D3D12_INPUT_ELEMENT_DESC> GetDX12InputElements(const RasterPipelineCreateInfo& createInfo)
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> result {};
        const auto& vertexState = createInfo.vertexState;

        for (auto i = 0; i < vertexState.bufferLayouts.size(); i++) {
            const auto& layout = vertexState.bufferLayouts[i];

            for (auto j = 0; j < layout.attributes.size(); j++) {
                const auto& attribute = layout.attributes[j];
                const auto& vertexBinding = std::get<HlslVertexBinding>(attribute.platformBinding);

                D3D12_INPUT_ELEMENT_DESC desc {};
                desc.Format = EnumCast<VertexFormat, DXGI_FORMAT>(attribute.format);
                desc.InputSlot = i;
                desc.InputSlotClass = EnumCast<VertexStepMode, D3D12_INPUT_CLASSIFICATION>(layout.stepMode);
                desc.AlignedByteOffset = attribute.offset;
                desc.SemanticName = vertexBinding.semanticName.c_str();
                desc.SemanticIndex = vertexBinding.semanticIndex;
                result.emplace_back(desc);
            }
        }
        return result;
    }

    void UpdateDX12InputLayoutDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const std::vector<D3D12_INPUT_ELEMENT_DESC>& inputElements)
    {
        desc.InputLayout.NumElements = inputElements.size();
        desc.InputLayout.pInputElementDescs = inputElements.data();
    }
}

namespace RHI::DirectX12 {
    DX12ComputePipeline::DX12ComputePipeline(DX12Device& inDevice, const ComputePipelineCreateInfo& inCreateInfo) : ComputePipeline(inCreateInfo), pipelineLayout(nullptr)
    {
        SavePipelineLayout(inCreateInfo);
        CreateNativeComputePipeline(inDevice, inCreateInfo);
    }

    DX12ComputePipeline::~DX12ComputePipeline() = default;

    DX12PipelineLayout& DX12ComputePipeline::GetPipelineLayout() const
    {
        return *pipelineLayout;
    }

    ID3D12PipelineState* DX12ComputePipeline::GetNative() const
    {
        return nativePipelineState.Get();
    }

    void DX12ComputePipeline::SavePipelineLayout(const ComputePipelineCreateInfo& createInfo)
    {
        auto* pl = static_cast<DX12PipelineLayout*>(createInfo.layout);
        Assert(pl);
        pipelineLayout = pl;
    }

    void DX12ComputePipeline::CreateNativeComputePipeline(DX12Device& inDevice, const ComputePipelineCreateInfo& inCreateInfo)
    {
        const auto* computeShader = static_cast<DX12ShaderModule*>(inCreateInfo.computeShader);

        D3D12_COMPUTE_PIPELINE_STATE_DESC desc {};
        desc.pRootSignature = pipelineLayout->GetNative();
        desc.CS = computeShader->GetNative();
        Assert(SUCCEEDED(inDevice.GetNative()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&nativePipelineState))));
    }

    DX12RasterPipeline::DX12RasterPipeline(DX12Device& inDevice, const RasterPipelineCreateInfo& inCreateInfo) : RasterPipeline(inCreateInfo), pipelineLayout(nullptr)
    {
        SavePipelineLayout(inCreateInfo);
        CreateNativeGraphicsPipeline(inDevice, inCreateInfo);
    }

    DX12RasterPipeline::~DX12RasterPipeline() = default;

    DX12PipelineLayout& DX12RasterPipeline::GetPipelineLayout() const
    {
        return *pipelineLayout;
    }

    ID3D12PipelineState* DX12RasterPipeline::GetNative() const
    {
        return nativePipelineState.Get();
    }

    void DX12RasterPipeline::SavePipelineLayout(const RasterPipelineCreateInfo& inCreateInfo)
    {
        auto* pl = static_cast<DX12PipelineLayout*>(inCreateInfo.layout);
        Assert(pl);
        pipelineLayout = pl;
    }

    void DX12RasterPipeline::CreateNativeGraphicsPipeline(DX12Device& inDevice, const RasterPipelineCreateInfo& inCreateInfo)
    {
        auto* vertexShader = static_cast<DX12ShaderModule*>(inCreateInfo.vertexShader);
        auto* fragmentShader = static_cast<DX12ShaderModule*>(inCreateInfo.pixelShader);

        D3D12_GRAPHICS_PIPELINE_STATE_DESC desc {};
        desc.pRootSignature = pipelineLayout->GetNative();
        desc.VS = vertexShader->GetNative();
        desc.PS = fragmentShader->GetNative();
        desc.RasterizerState = GetDX12RasterizerDesc(inCreateInfo);
        desc.BlendState = GetDX12BlendDesc(inCreateInfo);
        desc.DepthStencilState = GetDX12DepthStencilDesc(inCreateInfo);
        desc.SampleMask = GetDX12SampleMask(inCreateInfo);
        desc.SampleDesc = GetDX12SampleDesc(inCreateInfo);
        desc.PrimitiveTopologyType = EnumCast<PrimitiveTopologyType, D3D12_PRIMITIVE_TOPOLOGY_TYPE>(inCreateInfo.primitiveState.topologyType);
        UpdateDX12RenderTargetsDesc(desc, inCreateInfo);
        UpdateDX12DepthStencilTargetDesc(desc, inCreateInfo);
        auto inputElements = GetDX12InputElements(inCreateInfo);
        UpdateDX12InputLayoutDesc(desc, inputElements);

        bool success = SUCCEEDED(inDevice.GetNative()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&nativePipelineState)));
        Assert(success);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            Assert(SUCCEEDED(nativePipelineState->SetName(Common::StringUtils::ToWideString(inCreateInfo.debugName).c_str())));
        }
#endif
    }
}
