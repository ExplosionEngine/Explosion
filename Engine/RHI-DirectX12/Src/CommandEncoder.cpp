//
// Created by johnk on 2022/3/23.
//

#include <RHI/DirectX12/CommandEncoder.h>
#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/Pipeline.h>
#include <RHI/DirectX12/PipelineLayout.h>
#include <RHI/DirectX12/BindGroup.h>
#include <RHI/DirectX12/BindGroupLayout.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/BufferView.h>

namespace RHI::DirectX12 {
    DX12ComputePassCommandEncoder::DX12ComputePassCommandEncoder(DX12Device& device, DX12CommandBuffer& commandBuffer) : ComputePassCommandEncoder(), device(device), commandBuffer(commandBuffer) {}

    DX12ComputePassCommandEncoder::~DX12ComputePassCommandEncoder() = default;

    void DX12ComputePassCommandEncoder::SetPipeline(ComputePipeline* pipeline)
    {
        computePipeline = dynamic_cast<DX12ComputePipeline*>(pipeline);
        if (computePipeline == nullptr) {
            throw DX12Exception("pipeline cannot be nullptr");
        }

        commandBuffer.GetDX12GraphicsCommandList()->Reset(device.GetDX12CommandAllocator().Get(), computePipeline->GetDX12PipelineState().Get());
        commandBuffer.GetDX12GraphicsCommandList()->SetPipelineState(computePipeline->GetDX12PipelineState().Get());
        commandBuffer.GetDX12GraphicsCommandList()->SetGraphicsRootSignature(computePipeline->GetPipelineLayout().GetDX12RootSignature().Get());
    }

    void DX12ComputePassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* tBindGroup)
    {
        auto* bindGroup = dynamic_cast<DX12BindGroup*>(tBindGroup);
        auto& bindGroupLayout = bindGroup->GetBindGroupLayout();
        auto& pipelineLayout = computePipeline->GetPipelineLayout();

        if (layoutIndex != bindGroupLayout.GetLayoutIndex()) {
            throw DX12Exception("bad layout index");
        }
        if (computePipeline == nullptr) {
            throw DX12Exception("must cal SetPipeline() before SetBindGroup()");
        }

        const auto& bindings= bindGroup->GetBindings();
        for (const auto& binding : bindings) {
            ForEachBitsType<ShaderStageBits>([this, &binding, &pipelineLayout, layoutIndex](ShaderStageBits shaderStage) -> void {
                std::optional<BindingTypeAndRootParameterIndex> t = pipelineLayout.QueryRootDescriptorParameterIndex(shaderStage, layoutIndex, binding.first);
                if (!t.has_value()) {
                    return;
                }
                if (t.value().first != binding.second.first) {
                    throw DX12Exception(std::string("bad binding type on slot [layoutIndex: ") + std::to_string(layoutIndex) + ", binding: " + std::to_string(binding.first) + "]");
                }
                commandBuffer.GetDX12GraphicsCommandList()->SetGraphicsRootDescriptorTable(t.value().second, binding.second.second);
            });
        }
    }

    void DX12ComputePassCommandEncoder::Dispatch(size_t groupCountX, size_t groupCountY, size_t groupCountZ)
    {
        commandBuffer.GetDX12GraphicsCommandList()->Dispatch(groupCountX, groupCountY, groupCountZ);
    }

    void DX12ComputePassCommandEncoder::EndPass()
    {
        delete this;
    }

    DX12GraphicsPassCommandEncoder::DX12GraphicsPassCommandEncoder(DX12Device& device, DX12CommandBuffer& commandBuffer) : GraphicsPassCommandEncoder(), device(device), commandBuffer(commandBuffer) {}

    DX12GraphicsPassCommandEncoder::~DX12GraphicsPassCommandEncoder() = default;

    void DX12GraphicsPassCommandEncoder::SetPipeline(GraphicsPipeline* pipeline)
    {
        graphicsPipeline = dynamic_cast<DX12GraphicsPipeline*>(pipeline);

        commandBuffer.GetDX12GraphicsCommandList()->Reset(device.GetDX12CommandAllocator().Get(), graphicsPipeline->GetDX12PipelineState().Get());
        commandBuffer.GetDX12GraphicsCommandList()->SetPipelineState(graphicsPipeline->GetDX12PipelineState().Get());
        commandBuffer.GetDX12GraphicsCommandList()->SetGraphicsRootSignature(graphicsPipeline->GetPipelineLayout().GetDX12RootSignature().Get());
    }

    void DX12GraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* tBindGroup)
    {
        auto* bindGroup = dynamic_cast<DX12BindGroup*>(tBindGroup);
        auto& bindGroupLayout = bindGroup->GetBindGroupLayout();
        auto& pipelineLayout = graphicsPipeline->GetPipelineLayout();

        if (layoutIndex != bindGroupLayout.GetLayoutIndex()) {
            throw DX12Exception("bad layout index");
        }
        if (graphicsPipeline == nullptr) {
            throw DX12Exception("must cal SetPipeline() before SetBindGroup()");
        }

        const auto& bindings= bindGroup->GetBindings();
        for (const auto& binding : bindings) {
            ForEachBitsType<ShaderStageBits>([this, &binding, &pipelineLayout, layoutIndex](ShaderStageBits shaderStage) -> void {
                std::optional<BindingTypeAndRootParameterIndex> t = pipelineLayout.QueryRootDescriptorParameterIndex(shaderStage, layoutIndex, binding.first);
                if (!t.has_value()) {
                    return;
                }
                if (t.value().first != binding.second.first) {
                    throw DX12Exception(std::string("bad binding type on slot [layoutIndex: ") + std::to_string(layoutIndex) + ", binding: " + std::to_string(binding.first) + "]");
                }
                commandBuffer.GetDX12GraphicsCommandList()->SetGraphicsRootDescriptorTable(t.value().second, binding.second.second);
            });
        }
    }

    void DX12GraphicsPassCommandEncoder::SetIndexBuffer(BufferView* tBufferView)
    {
        auto* bufferView = dynamic_cast<DX12BufferView*>(tBufferView);
        commandBuffer.GetDX12GraphicsCommandList()->IASetIndexBuffer(&bufferView->GetDX12IndexBufferView());
    }

    void DX12GraphicsPassCommandEncoder::SetVertexBuffer(size_t slot, BufferView* tBufferView)
    {
        auto* bufferView = dynamic_cast<DX12BufferView*>(tBufferView);
        commandBuffer.GetDX12GraphicsCommandList()->IASetVertexBuffers(slot, 1, &bufferView->GetDX12VertexBufferView());
    }

    void DX12GraphicsPassCommandEncoder::Draw(size_t vertexCount, size_t instanceCount, size_t firstVertex, size_t firstInstance)
    {
        commandBuffer.GetDX12GraphicsCommandList()->DrawInstanced(vertexCount, instanceCount, firstVertex, firstInstance);
    }

    void DX12GraphicsPassCommandEncoder::DrawIndexed(size_t indexCount, size_t instanceCount, size_t firstIndex, size_t baseVertex, size_t firstInstance)
    {
        commandBuffer.GetDX12GraphicsCommandList()->DrawIndexedInstanced(indexCount, instanceCount, firstIndex, baseVertex, firstInstance);
    }

    void DX12GraphicsPassCommandEncoder::SetViewport(float x, float y, float width, float height, float minDepth, float maxDepth)
    {
        // (x, y) = topLeft
        CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(x, y, width, height, minDepth, maxDepth);
        commandBuffer.GetDX12GraphicsCommandList()->RSSetViewports(1, &viewport);
    }

    void DX12GraphicsPassCommandEncoder::SetScissor(uint32_t left, uint32_t top, uint32_t right, uint32_t bottom)
    {
        CD3DX12_RECT scissor = CD3DX12_RECT(left, top, right, bottom);
        commandBuffer.GetDX12GraphicsCommandList()->RSSetScissorRects(1, &scissor);
    }

    void DX12GraphicsPassCommandEncoder::SetBlendConstant(const float* constants)
    {
        commandBuffer.GetDX12GraphicsCommandList()->OMSetBlendFactor(constants);
    }

    void DX12GraphicsPassCommandEncoder::SetStencilReference(uint32_t reference)
    {
        commandBuffer.GetDX12GraphicsCommandList()->OMSetStencilRef(reference);
    }

    void DX12GraphicsPassCommandEncoder::EndPass()
    {
        delete this;
    }

    DX12CommandEncoder::DX12CommandEncoder(DX12Device& device, DX12CommandBuffer& commandBuffer) : CommandEncoder(), device(device), commandBuffer(commandBuffer) {}

    DX12CommandEncoder::~DX12CommandEncoder() = default;

    void DX12CommandEncoder::CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo , const Extent<3>& size)
    {
        // TODO
    }

    ComputePassCommandEncoder* DX12CommandEncoder::BeginComputePass()
    {
        return new DX12ComputePassCommandEncoder(device, commandBuffer);
    }

    GraphicsPassCommandEncoder* DX12CommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        return new DX12GraphicsPassCommandEncoder(device, commandBuffer);
    }

    void DX12CommandEncoder::End()
    {
        commandBuffer.GetDX12GraphicsCommandList()->Close();
    }
}
