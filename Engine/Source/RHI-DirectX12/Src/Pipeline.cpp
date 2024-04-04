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
    uint8_t GetDX12RenderTargetWriteMasks(ColorWriteFlags writeFlags)
    {
        static std::unordered_map<ColorWriteBits, uint8_t> MAP = {
            { ColorWriteBits::red, D3D12_COLOR_WRITE_ENABLE_RED },
            { ColorWriteBits::green, D3D12_COLOR_WRITE_ENABLE_GREEN },
            { ColorWriteBits::blue, D3D12_COLOR_WRITE_ENABLE_BLUE },
            { ColorWriteBits::alpha, D3D12_COLOR_WRITE_ENABLE_ALPHA }
        };

        uint8_t result = 0;
        for (auto iter : MAP) {
            if (writeFlags & iter.first) {
                result |= iter.second;
            }
        }
        return result;
    }

    D3D12_RENDER_TARGET_BLEND_DESC GetDX12RenderTargetBlendDesc(const ColorTargetState& colorTargetState)
    {
        D3D12_RENDER_TARGET_BLEND_DESC desc {};
        desc.BlendEnable = true;
        desc.LogicOpEnable = false;
        desc.RenderTargetWriteMask = GetDX12RenderTargetWriteMasks(colorTargetState.writeFlags);
        desc.BlendOp = DX12EnumCast<BlendOp, D3D12_BLEND_OP>(colorTargetState.blend.color.op);
        desc.SrcBlend = DX12EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.blend.color.srcFactor);
        desc.DestBlend = DX12EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.blend.color.dstFactor);
        desc.BlendOpAlpha = DX12EnumCast<BlendOp, D3D12_BLEND_OP>(colorTargetState.blend.alpha.op);
        desc.SrcBlendAlpha = DX12EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.blend.alpha.srcFactor);
        desc.DestBlendAlpha = DX12EnumCast<BlendFactor, D3D12_BLEND>(colorTargetState.blend.alpha.dstFactor);
        return desc;
    }

    CD3DX12_RASTERIZER_DESC GetDX12RasterizerDesc(const RasterPipelineCreateInfo& createInfo)
    {
        CD3DX12_RASTERIZER_DESC desc(D3D12_DEFAULT);
        // TODO expose to RHI interface?
        desc.FillMode = D3D12_FILL_MODE_SOLID;
        desc.CullMode = DX12EnumCast<CullMode, D3D12_CULL_MODE>(createInfo.primitiveState.cullMode);
        desc.FrontCounterClockwise = createInfo.primitiveState.frontFace == FrontFace::ccw;
        desc.DepthBias = createInfo.depthStencilState.depthBias;
        desc.DepthBiasClamp = createInfo.depthStencilState.depthBiasClamp;
        desc.SlopeScaledDepthBias = createInfo.depthStencilState.depthBiasSlopeScale;
        desc.DepthClipEnable = createInfo.primitiveState.depthClip;
        // TODO check this
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
        desc.StencilFailOp = DX12EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.failOp);
        desc.StencilDepthFailOp = DX12EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.depthFailOp);
        desc.StencilPassOp = DX12EnumCast<StencilOp, D3D12_STENCIL_OP>(stencilFaceState.passOp);
        desc.StencilFunc = DX12EnumCast<ComparisonFunc, D3D12_COMPARISON_FUNC>(stencilFaceState.comparisonFunc);
        return desc;
    }

    CD3DX12_DEPTH_STENCIL_DESC GetDX12DepthStencilDesc(const RasterPipelineCreateInfo& createInfo)
    {
        CD3DX12_DEPTH_STENCIL_DESC desc(D3D12_DEFAULT);
        // TODO check this
        desc.DepthEnable = createInfo.depthStencilState.depthEnable;
        desc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
        desc.DepthFunc = DX12EnumCast<ComparisonFunc, D3D12_COMPARISON_FUNC>(createInfo.depthStencilState.depthComparisonFunc);
        desc.StencilEnable = createInfo.depthStencilState.stencilEnable;
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
        // TODO https://docs.microsoft.com/en-us/windows/win32/api/dxgicommon/ns-dxgicommon-dxgi_sample_desc
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
            desc.RTVFormats[i] = DX12EnumCast<PixelFormat, DXGI_FORMAT>(createInfo.fragmentState.colorTargets[i].format);
        }
    }

    void UpdateDX12DepthStencilTargetDesc(D3D12_GRAPHICS_PIPELINE_STATE_DESC& desc, const RasterPipelineCreateInfo& createInfo)
    {
        if (!createInfo.depthStencilState.depthEnable && !createInfo.depthStencilState.stencilEnable) {
            return;
        }
        desc.DSVFormat = DX12EnumCast<PixelFormat, DXGI_FORMAT>(createInfo.depthStencilState.format);
    }

    std::vector<D3D12_INPUT_ELEMENT_DESC> GetDX12InputElements(const RasterPipelineCreateInfo& createInfo)
    {
        std::vector<D3D12_INPUT_ELEMENT_DESC> result {};
        const auto& vertexState = createInfo.vertexState;

        for (auto i = 0; i < vertexState.bufferLayouts.size(); i++) {
            const auto& layout = vertexState.bufferLayouts[i];

            for (auto j = 0; j < layout.attributes.size(); j++) {
                const auto& attribute = layout.attributes[j];

                D3D12_INPUT_ELEMENT_DESC desc {};
                desc.Format = DX12EnumCast<VertexFormat, DXGI_FORMAT>(attribute.format);
                desc.InputSlot = i;
                desc.InputSlotClass = DX12EnumCast<VertexStepMode, D3D12_INPUT_CLASSIFICATION>(layout.stepMode);
                desc.AlignedByteOffset = attribute.offset;
                desc.SemanticName = attribute.semanticName.c_str();
                desc.SemanticIndex = attribute.semanticIndex;
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

    void DX12ComputePipeline::Destroy()
    {
        delete this;
    }

    DX12PipelineLayout& DX12ComputePipeline::GetPipelineLayout()
    {
        return *pipelineLayout;
    }

    ID3D12PipelineState* DX12ComputePipeline::GetNative()
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
        auto* computeShader = static_cast<DX12ShaderModule*>(inCreateInfo.computeShader);

        D3D12_COMPUTE_PIPELINE_STATE_DESC desc {};
        desc.pRootSignature = pipelineLayout->GetNative();
        desc.CS = computeShader->GetNative();

        bool success = SUCCEEDED(inDevice.GetNative()->CreateComputePipelineState(&desc, IID_PPV_ARGS(&nativePipelineState)));
        Assert(success);
    }

    DX12RasterPipeline::DX12RasterPipeline(DX12Device& inDevice, const RasterPipelineCreateInfo& inCreateInfo) : RasterPipeline(inCreateInfo), pipelineLayout(nullptr)
    {
        SavePipelineLayout(inCreateInfo);
        CreateNativeGraphicsPipeline(inDevice, inCreateInfo);
    }

    DX12RasterPipeline::~DX12RasterPipeline() = default;

    void DX12RasterPipeline::Destroy()
    {
        delete this;
    }

    DX12PipelineLayout& DX12RasterPipeline::GetPipelineLayout()
    {
        return *pipelineLayout;
    }

    ID3D12PipelineState* DX12RasterPipeline::GetNative()
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
        desc.PrimitiveTopologyType = DX12EnumCast<PrimitiveTopologyType, D3D12_PRIMITIVE_TOPOLOGY_TYPE>(inCreateInfo.primitiveState.topologyType);
        UpdateDX12RenderTargetsDesc(desc, inCreateInfo);
        UpdateDX12DepthStencilTargetDesc(desc, inCreateInfo);
        auto inputElements = GetDX12InputElements(inCreateInfo);
        UpdateDX12InputLayoutDesc(desc, inputElements);

        bool success = SUCCEEDED(inDevice.GetNative()->CreateGraphicsPipelineState(&desc, IID_PPV_ARGS(&nativePipelineState)));
        Assert(success);

#if BUILD_CONFIG_DEBUG
        if (!inCreateInfo.debugName.empty()) {
            nativePipelineState->SetName(Common::StringUtils::ToWideString(inCreateInfo.debugName).c_str());
        }
#endif
    }
}
