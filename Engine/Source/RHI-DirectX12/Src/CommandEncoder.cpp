//
// Created by johnk on 2022/3/23.
//

#include <optional>

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
#include <RHI/DirectX12/Texture.h>
#include <RHI/DirectX12/TextureView.h>
#include <RHI/Synchronous.h>

namespace RHI::DirectX12 {
    D3D12_CLEAR_FLAGS GetDX12ClearFlags(const GraphicsPassDepthStencilAttachment& depthStencilAttachment)
    {
        D3D12_CLEAR_FLAGS result;
        if (depthStencilAttachment.depthLoadOp == LoadOp::CLEAR) {
            result |= D3D12_CLEAR_FLAG_DEPTH;
        }
        if (depthStencilAttachment.stencilLoadOp == LoadOp::CLEAR) {
            result |= D3D12_CLEAR_FLAG_STENCIL;
        }
        return result;
    }
}

namespace RHI::DirectX12 {
    DX12ComputePassCommandEncoder::DX12ComputePassCommandEncoder(DX12Device& device, DX12CommandBuffer& commandBuffer) : ComputePassCommandEncoder(), device(device), commandBuffer(commandBuffer) {}

    DX12ComputePassCommandEncoder::~DX12ComputePassCommandEncoder() = default;

    void DX12ComputePassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* tBindGroup)
    {
        auto* bindGroup = dynamic_cast<DX12BindGroup*>(tBindGroup);
        auto& bindGroupLayout = bindGroup->GetBindGroupLayout();
        auto& pipelineLayout = computePipeline->GetPipelineLayout();

        Assert(layoutIndex == bindGroupLayout.GetLayoutIndex());
        Assert(computePipeline);

        const auto& bindings= bindGroup->GetBindings();
        for (const auto& binding : bindings) {
            ForEachBitsType<ShaderStageBits>([this, &binding, &pipelineLayout, layoutIndex](ShaderStageBits shaderStage) -> void {
                std::optional<BindingTypeAndRootParameterIndex> t = pipelineLayout.QueryRootDescriptorParameterIndex(shaderStage, layoutIndex, binding.first);
                if (!t.has_value()) {
                    return;
                }
                Assert(t.value().first == binding.second.first);
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

    DX12GraphicsPassCommandEncoder::DX12GraphicsPassCommandEncoder(DX12Device& device, DX12CommandBuffer& commandBuffer, const GraphicsPassBeginInfo* beginInfo) : GraphicsPassCommandEncoder(), device(device), commandBuffer(commandBuffer), graphicsPipeline(nullptr)
    {
        // set render targets
        std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles(beginInfo->colorAttachmentNum);
        for (auto i = 0; i < rtvHandles.size(); i++) {
            auto* view = dynamic_cast<DX12TextureView*>(beginInfo->colorAttachments[i].view);
            Assert(view);
            rtvHandles[i] = view->GetDX12CpuDescriptorHandle();
        }
        std::optional<CD3DX12_CPU_DESCRIPTOR_HANDLE> dsvHandle;
        if (beginInfo->depthStencilAttachment != nullptr) {
            auto* view = dynamic_cast<DX12TextureView*>(beginInfo->depthStencilAttachment->view);
            Assert(view);
            dsvHandle = view->GetDX12CpuDescriptorHandle();
        }
        commandBuffer.GetDX12GraphicsCommandList()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), false, dsvHandle.has_value() ? &dsvHandle.value() : nullptr);

        // clear render targets
        for (auto i = 0; i < rtvHandles.size(); i++) {
            const auto& colorAttachment = beginInfo->colorAttachments[i];
            if (colorAttachment.loadOp != LoadOp::CLEAR) {
                continue;
            }
            const auto* clearValue = reinterpret_cast<const float*>(&colorAttachment.clearValue);
            commandBuffer.GetDX12GraphicsCommandList()->ClearRenderTargetView(rtvHandles[i], clearValue, 0, nullptr);
        }
        if (dsvHandle.has_value()) {
            const auto& depthStencilAttachment = *beginInfo->depthStencilAttachment;
            if (depthStencilAttachment.depthLoadOp != LoadOp::CLEAR && depthStencilAttachment.stencilLoadOp != LoadOp::CLEAR) {
                return;
            }
            commandBuffer.GetDX12GraphicsCommandList()->ClearDepthStencilView(dsvHandle.value(), GetDX12ClearFlags(depthStencilAttachment), depthStencilAttachment.depthClearValue, depthStencilAttachment.stencilClearValue, 0, nullptr);
        }
    }

    DX12GraphicsPassCommandEncoder::~DX12GraphicsPassCommandEncoder() = default;

    void DX12GraphicsPassCommandEncoder::SetBindGroup(uint8_t layoutIndex, BindGroup* tBindGroup)
    {
        auto* bindGroup = dynamic_cast<DX12BindGroup*>(tBindGroup);
        auto& bindGroupLayout = bindGroup->GetBindGroupLayout();
        auto& pipelineLayout = graphicsPipeline->GetPipelineLayout();

        Assert(layoutIndex == bindGroupLayout.GetLayoutIndex());
        Assert(graphicsPipeline);

        const auto& bindings= bindGroup->GetBindings();
        for (const auto& binding : bindings) {
            ForEachBitsType<ShaderStageBits>([this, &binding, &pipelineLayout, layoutIndex](ShaderStageBits shaderStage) -> void {
                std::optional<BindingTypeAndRootParameterIndex> t = pipelineLayout.QueryRootDescriptorParameterIndex(shaderStage, layoutIndex, binding.first);
                if (!t.has_value()) {
                    return;
                }
                Assert(t.value().first == binding.second.first);
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

    void DX12GraphicsPassCommandEncoder::SetPrimitiveTopology(PrimitiveTopology primitiveTopology)
    {
        commandBuffer.GetDX12GraphicsCommandList()->IASetPrimitiveTopology(DX12EnumCast<PrimitiveTopology, D3D_PRIMITIVE_TOPOLOGY>(primitiveTopology));
    }

    void DX12GraphicsPassCommandEncoder::SetStencilReference(uint32_t reference)
    {
        commandBuffer.GetDX12GraphicsCommandList()->OMSetStencilRef(reference);
    }

    void DX12GraphicsPassCommandEncoder::EndPass()
    {
        delete this;
    }

    DX12CommandEncoder::DX12CommandEncoder(DX12Device& device, DX12CommandBuffer& commandBuffer) : CommandEncoder(), device(device), commandBuffer(commandBuffer)
    {
        commandBuffer.GetDX12GraphicsCommandList()->Reset(device.GetDX12CommandAllocator().Get(), nullptr);
    }

    DX12CommandEncoder::~DX12CommandEncoder() = default;

    void DX12CommandEncoder::CopyBufferToBuffer(Buffer* src, size_t srcOffset, Buffer* dst, size_t dstOffset, size_t size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyBufferToTexture(Buffer* src, Texture* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        auto* buffer = dynamic_cast<DX12Buffer*>(src);
        auto* texture = dynamic_cast<DX12Texture*>(dst);
        auto origin = subResourceInfo->origin;

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
        layout.Offset = 0;
        layout.Footprint.Depth = size.z;
        layout.Footprint.Width = size.x;
        layout.Footprint.Height = size.y;
        layout.Footprint.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(texture->GetFormat()); // Can this attrib of buffer retrive from texture?
        layout.Footprint.RowPitch = 4 * size.x; // TODO get pixel component nums

        CD3DX12_TEXTURE_COPY_LOCATION dest(texture->GetDX12Resource().Get(), 0);
        CD3DX12_TEXTURE_COPY_LOCATION source(buffer->GetDX12Resource().Get(), layout);
        commandBuffer.GetDX12GraphicsCommandList()->CopyTextureRegion(&dest, origin.x, origin.y, origin.z, &source, nullptr);
    }

    void DX12CommandEncoder::CopyTextureToBuffer(Texture* src, Buffer* dst, const TextureSubResourceInfo* subResourceInfo, const Extent<3>& size)
    {
        // TODO
    }

    void DX12CommandEncoder::CopyTextureToTexture(Texture* src, const TextureSubResourceInfo* srcSubResourceInfo, Texture* dst, const TextureSubResourceInfo* dstSubResourceInfo , const Extent<3>& size)
    {
        // TODO
    }

    void DX12CommandEncoder::ResourceBarrier(const Barrier& barrier)
    {
        ID3D12Resource* resource;
        D3D12_RESOURCE_STATES beforeState;
        D3D12_RESOURCE_STATES afterState;
        if (barrier.type == ResourceType::BUFFER) {
            auto* buffer = dynamic_cast<DX12Buffer*>(barrier.buffer.pointer);
            Assert(buffer);
            resource = buffer->GetDX12Resource().Get();
            beforeState = DX12EnumCast<BufferState, D3D12_RESOURCE_STATES>(barrier.buffer.before);
            afterState = DX12EnumCast<BufferState, D3D12_RESOURCE_STATES>(barrier.buffer.after);
        } else {
            auto* texture = dynamic_cast<DX12Texture*>(barrier.texture.pointer);
            Assert(texture);
            resource = texture->GetDX12Resource().Get();
            beforeState = DX12EnumCast<TextureState, D3D12_RESOURCE_STATES>(barrier.texture.before);
            afterState = DX12EnumCast<TextureState, D3D12_RESOURCE_STATES>(barrier.texture.after);
        }

        CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, beforeState, afterState);
        commandBuffer.GetDX12GraphicsCommandList()->ResourceBarrier(1, &resourceBarrier);
    }

    ComputePassCommandEncoder* DX12CommandEncoder::BeginComputePass(const ComputePassBeginInfo* beginInfo)
    {
        auto* computePipeline = dynamic_cast<DX12ComputePipeline*>(beginInfo->pipeline);
        Assert(computePipeline);

        auto* result = new DX12ComputePassCommandEncoder(device, commandBuffer);
        commandBuffer.GetDX12GraphicsCommandList()->SetPipelineState(computePipeline->GetDX12PipelineState().Get());
        commandBuffer.GetDX12GraphicsCommandList()->SetGraphicsRootSignature(computePipeline->GetPipelineLayout().GetDX12RootSignature().Get());
        return result;
    }

    GraphicsPassCommandEncoder* DX12CommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo* beginInfo)
    {
        auto* graphicsPipeline = dynamic_cast<DX12GraphicsPipeline*>(beginInfo->pipeline);
        Assert(graphicsPipeline);

        auto* result = new DX12GraphicsPassCommandEncoder(device, commandBuffer, beginInfo);
        commandBuffer.GetDX12GraphicsCommandList()->SetPipelineState(graphicsPipeline->GetDX12PipelineState().Get());
        commandBuffer.GetDX12GraphicsCommandList()->SetGraphicsRootSignature(graphicsPipeline->GetPipelineLayout().GetDX12RootSignature().Get());
        return result;
    }

    void DX12CommandEncoder::End()
    {
        commandBuffer.GetDX12GraphicsCommandList()->Close();
        delete this;
    }
}
