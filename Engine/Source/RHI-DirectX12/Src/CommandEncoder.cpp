//
// Created by johnk on 2022/3/23.
//

#include <optional>
#include <array>

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
    static D3D12_CLEAR_FLAGS GetDX12ClearFlags(const GraphicsPassDepthStencilAttachment& depthStencilAttachment)
    {
        Assert(depthStencilAttachment.depthLoadOp == LoadOp::clear || depthStencilAttachment.stencilLoadOp == LoadOp::clear);

        if (depthStencilAttachment.stencilLoadOp != LoadOp::clear) {
            return D3D12_CLEAR_FLAG_DEPTH;
        }
        if (depthStencilAttachment.depthLoadOp != LoadOp::clear) {
            return D3D12_CLEAR_FLAG_STENCIL;
        }
        return D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL;
    }
}

namespace RHI::DirectX12 {
    DX12CopyPassCommandEncoder::DX12CopyPassCommandEncoder(DX12Device& inDevice, DX12CommandEncoder& inCmdEncoder, DX12CommandBuffer& inCmdBuffer)
        : device(inDevice)
        , commandEncoder(inCmdEncoder)
        , commandBuffer(inCmdBuffer)
    {
    }

    DX12CopyPassCommandEncoder::~DX12CopyPassCommandEncoder() = default;

    void DX12CopyPassCommandEncoder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandEncoder.ResourceBarrier(inBarrier);
    }

    void DX12CopyPassCommandEncoder::CopyBufferToBuffer(Buffer* inSrcBuffer, size_t inSrcOffset, Buffer* inDestBuffer, size_t inDestOffset, size_t inSize)
    {
        // TODO
    }

    void DX12CopyPassCommandEncoder::CopyBufferToTexture(Buffer* inSrcBuffer, Texture* inDestTexture, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize)
    {
        auto* buffer = static_cast<DX12Buffer*>(inSrcBuffer);
        auto* texture = static_cast<DX12Texture*>(inDestTexture);
        auto origin = inSubResourceInfo->origin;

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
        layout.Offset = 0;
        layout.Footprint.Depth = inSize.z;
        layout.Footprint.Width = inSize.x;
        layout.Footprint.Height = inSize.y;
        layout.Footprint.Format = DX12EnumCast<PixelFormat, DXGI_FORMAT>(texture->GetFormat());
        layout.Footprint.RowPitch = inSize.x * GetBytesPerPixel(texture->GetFormat()); // row pitch must be a multiple of 256, let dx checks if the texture is valid

        CD3DX12_TEXTURE_COPY_LOCATION dest(texture->GetNative(), 0);
        CD3DX12_TEXTURE_COPY_LOCATION source(buffer->GetNative(), layout);
        commandBuffer.GetNative()->CopyTextureRegion(&dest, origin.x, origin.y, origin.z, &source, nullptr);
    }

    void DX12CopyPassCommandEncoder::CopyTextureToBuffer(Texture* inSrcTexture, Buffer* inDestBuffer, const TextureSubResourceInfo* inSubResourceInfo, const Common::UVec3& inSize)
    {
        // TODO
    }

    void DX12CopyPassCommandEncoder::CopyTextureToTexture(Texture* inSrcTexture, const TextureSubResourceInfo* inSrcSubResourceInfo, Texture* inDestTexture, const TextureSubResourceInfo* inDestSubResourceInfo , const Common::UVec3& inSize)
    {
        // TODO
    }

    void DX12CopyPassCommandEncoder::EndPass()
    {
    }

    void DX12CopyPassCommandEncoder::Destroy()
    {
        delete this;
    }

    DX12ComputePassCommandEncoder::DX12ComputePassCommandEncoder(DX12Device& inDevice, DX12CommandEncoder& inCmdEncoder, DX12CommandBuffer& inCmdBuffer)
        : ComputePassCommandEncoder()
        , device(inDevice)
        , commandEncoder(inCmdEncoder)
        , commandBuffer(inCmdBuffer)
    {
    }

    DX12ComputePassCommandEncoder::~DX12ComputePassCommandEncoder() = default;

    void DX12ComputePassCommandEncoder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandEncoder.ResourceBarrier(inBarrier);
    }

    void DX12ComputePassCommandEncoder::SetPipeline(ComputePipeline* inPipeline)
    {
        computePipeline = static_cast<DX12ComputePipeline*>(inPipeline);
        Assert(computePipeline);

        commandBuffer.GetNative()->SetPipelineState(computePipeline->GetNative());
        commandBuffer.GetNative()->SetGraphicsRootSignature(computePipeline->GetPipelineLayout().GetNative());
    }

    void DX12ComputePassCommandEncoder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
    {
        auto* bindGroup = static_cast<DX12BindGroup*>(inBindGroup);
        auto& bindGroupLayout = bindGroup->GetBindGroupLayout();
        auto& pipelineLayout = computePipeline->GetPipelineLayout();

        Assert(inLayoutIndex == bindGroupLayout.GetLayoutIndex());
        Assert(computePipeline);

        const auto& bindings= bindGroup->GetNativeBindings();
        for (const auto& binding : bindings) {
            std::optional<BindingTypeAndRootParameterIndex> t = pipelineLayout.QueryRootDescriptorParameterIndex(inLayoutIndex, binding.first);
            if (!t.has_value()) {
                return;
            }
            commandBuffer.GetNative()->SetGraphicsRootDescriptorTable(t.value().second, commandBuffer.GetRuntimeDescriptorHeaps()->NewGpuDescriptorHandle(binding.first.rangeType, binding.second));
        }
    }

    void DX12ComputePassCommandEncoder::Dispatch(size_t inGroupCountX, size_t inGroupCountY, size_t inGroupCountZ)
    {
        commandBuffer.GetNative()->Dispatch(inGroupCountX, inGroupCountY, inGroupCountZ);
    }

    void DX12ComputePassCommandEncoder::EndPass()
    {
    }

    void DX12ComputePassCommandEncoder::Destroy()
    {
        delete this;
    }

    DX12GraphicsPassCommandEncoder::DX12GraphicsPassCommandEncoder(DX12Device& inDevice, DX12CommandEncoder& inCmdEncoder, DX12CommandBuffer& inCmdBuffer, const GraphicsPassBeginInfo& inBeginInfo)
        : GraphicsPassCommandEncoder()
        , device(inDevice)
        , commandEncoder(inCmdEncoder)
        , commandBuffer(inCmdBuffer)
        , graphicsPipeline(nullptr)
    {
        // set render targets
        std::vector<CD3DX12_CPU_DESCRIPTOR_HANDLE> rtvHandles(inBeginInfo.colorAttachments.size());
        for (auto i = 0; i < rtvHandles.size(); i++) {
            auto* view = static_cast<DX12TextureView*>(inBeginInfo.colorAttachments[i].view);
            Assert(view);
            rtvHandles[i] = view->GetNativeCpuDescriptorHandle();
        }
        std::optional<CD3DX12_CPU_DESCRIPTOR_HANDLE> dsvHandle;
        if (inBeginInfo.depthStencilAttachment.has_value()) {
            auto* view = static_cast<DX12TextureView*>(inBeginInfo.depthStencilAttachment->view);
            Assert(view);
            dsvHandle = view->GetNativeCpuDescriptorHandle();
        }
        inCmdBuffer.GetNative()->OMSetRenderTargets(rtvHandles.size(), rtvHandles.data(), false, dsvHandle.has_value() ? &dsvHandle.value() : nullptr);

        // clear render targets
        for (auto i = 0; i < rtvHandles.size(); i++) {
            const auto& colorAttachment = inBeginInfo.colorAttachments[i];
            if (colorAttachment.loadOp != LoadOp::clear) {
                continue;
            }
            const auto* clearValue = reinterpret_cast<const float*>(&colorAttachment.clearValue);
            inCmdBuffer.GetNative()->ClearRenderTargetView(rtvHandles[i], clearValue, 0, nullptr);
        }
        if (dsvHandle.has_value()) {
            const auto& depthStencilAttachment = *inBeginInfo.depthStencilAttachment;
            if (depthStencilAttachment.depthLoadOp != LoadOp::clear && depthStencilAttachment.stencilLoadOp != LoadOp::clear) {
                return;
            }
            inCmdBuffer.GetNative()->ClearDepthStencilView(dsvHandle.value(), GetDX12ClearFlags(depthStencilAttachment), depthStencilAttachment.depthClearValue, depthStencilAttachment.stencilClearValue, 0, nullptr);
        }
    }

    DX12GraphicsPassCommandEncoder::~DX12GraphicsPassCommandEncoder() = default;

    void DX12GraphicsPassCommandEncoder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandEncoder.ResourceBarrier(inBarrier);
    }

    void DX12GraphicsPassCommandEncoder::SetPipeline(GraphicsPipeline* inPipeline)
    {
        graphicsPipeline = static_cast<DX12GraphicsPipeline*>(inPipeline);
        Assert(graphicsPipeline);

        commandBuffer.GetNative()->SetPipelineState(graphicsPipeline->GetNative());
        commandBuffer.GetNative()->SetGraphicsRootSignature(graphicsPipeline->GetPipelineLayout().GetNative());
    }

    void DX12GraphicsPassCommandEncoder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
    {
        auto* bindGroup = static_cast<DX12BindGroup*>(inBindGroup);
        auto& bindGroupLayout = bindGroup->GetBindGroupLayout();
        auto& pipelineLayout = graphicsPipeline->GetPipelineLayout();

        Assert(inLayoutIndex == bindGroupLayout.GetLayoutIndex());
        Assert(graphicsPipeline);

        const auto& bindings= bindGroup->GetNativeBindings();
        for (const auto& binding : bindings) {
            std::optional<BindingTypeAndRootParameterIndex> t = pipelineLayout.QueryRootDescriptorParameterIndex(inLayoutIndex, binding.first);
            if (!t.has_value()) {
                return;
            }
            commandBuffer.GetNative()->SetGraphicsRootDescriptorTable(t.value().second, commandBuffer.GetRuntimeDescriptorHeaps()->NewGpuDescriptorHandle(binding.first.rangeType, binding.second));
        }
    }

    void DX12GraphicsPassCommandEncoder::SetIndexBuffer(BufferView* inBufferView)
    {
        auto* bufferView = static_cast<DX12BufferView*>(inBufferView);
        commandBuffer.GetNative()->IASetIndexBuffer(&bufferView->GetNativeIndexBufferView());
    }

    void DX12GraphicsPassCommandEncoder::SetVertexBuffer(size_t inSlot, BufferView* inBufferView)
    {
        auto* bufferView = static_cast<DX12BufferView*>(inBufferView);
        commandBuffer.GetNative()->IASetVertexBuffers(inSlot, 1, &bufferView->GetNativeVertexBufferView());
    }

    void DX12GraphicsPassCommandEncoder::Draw(size_t inVertexCount, size_t inInstanceCount, size_t inFirstVertex, size_t inFirstInstance)
    {
        commandBuffer.GetNative()->DrawInstanced(inVertexCount, inInstanceCount, inFirstVertex, inFirstInstance);
    }

    void DX12GraphicsPassCommandEncoder::DrawIndexed(size_t inIndexCount, size_t inInstanceCount, size_t inFirstIndex, size_t inBaseVertex, size_t inFirstInstance)
    {
        commandBuffer.GetNative()->DrawIndexedInstanced(inIndexCount, inInstanceCount, inFirstIndex, inBaseVertex, inFirstInstance);
    }

    void DX12GraphicsPassCommandEncoder::SetViewport(float inX, float inY, float inWidth, float inHeight, float inMinDepth, float inMaxDepth)
    {
        // (x, y) = topLeft
        CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(inX, inY, inWidth, inHeight, inMinDepth, inMaxDepth);
        commandBuffer.GetNative()->RSSetViewports(1, &viewport);
    }

    void DX12GraphicsPassCommandEncoder::SetScissor(uint32_t inLeft, uint32_t inTop, uint32_t inRight, uint32_t inBottom)
    {
        CD3DX12_RECT scissor = CD3DX12_RECT(inLeft, inTop, inRight, inBottom);
        commandBuffer.GetNative()->RSSetScissorRects(1, &scissor);
    }

    void DX12GraphicsPassCommandEncoder::SetBlendConstant(const float* inConstants)
    {
        commandBuffer.GetNative()->OMSetBlendFactor(inConstants);
    }

    void DX12GraphicsPassCommandEncoder::SetPrimitiveTopology(PrimitiveTopology inPrimitiveTopology)
    {
        commandBuffer.GetNative()->IASetPrimitiveTopology(DX12EnumCast<PrimitiveTopology, D3D_PRIMITIVE_TOPOLOGY>(inPrimitiveTopology));
    }

    void DX12GraphicsPassCommandEncoder::SetStencilReference(uint32_t inReference)
    {
        commandBuffer.GetNative()->OMSetStencilRef(inReference);
    }

    void DX12GraphicsPassCommandEncoder::EndPass()
    {
    }

    void DX12GraphicsPassCommandEncoder::Destroy()
    {
        delete this;
    }

    DX12CommandEncoder::DX12CommandEncoder(DX12Device& inDevice, DX12CommandBuffer& inCmdBuffer) : CommandEncoder(), device(inDevice), commandBuffer(inCmdBuffer)
    {
        inCmdBuffer.GetNative()->Reset(inDevice.GetNativeCmdAllocator(), nullptr);

        inCmdBuffer.GetRuntimeDescriptorHeaps()->ResetUsed();
        auto activeHeap = inCmdBuffer.GetRuntimeDescriptorHeaps()->GetNative();
        inCmdBuffer.GetNative()->SetDescriptorHeaps(activeHeap.size(), activeHeap.data());
    }

    DX12CommandEncoder::~DX12CommandEncoder() = default;

    void DX12CommandEncoder::ResourceBarrier(const Barrier& inBarrier)
    {
        ID3D12Resource* resource;
        D3D12_RESOURCE_STATES beforeState;
        D3D12_RESOURCE_STATES afterState;
        if (inBarrier.type == ResourceType::buffer) {
            auto* buffer = static_cast<DX12Buffer*>(inBarrier.buffer.pointer);
            Assert(buffer);
            resource = buffer->GetNative();
            beforeState = DX12EnumCast<BufferState, D3D12_RESOURCE_STATES>(inBarrier.buffer.before);
            afterState = DX12EnumCast<BufferState, D3D12_RESOURCE_STATES>(inBarrier.buffer.after);
        } else {
            auto* texture = static_cast<DX12Texture*>(inBarrier.texture.pointer);
            Assert(texture);
            resource = texture->GetNative();
            beforeState = DX12EnumCast<TextureState, D3D12_RESOURCE_STATES>(inBarrier.texture.before);
            afterState = DX12EnumCast<TextureState, D3D12_RESOURCE_STATES>(inBarrier.texture.after);
        }

        CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, beforeState, afterState);
        commandBuffer.GetNative()->ResourceBarrier(1, &resourceBarrier);
    }

    CopyPassCommandEncoder* DX12CommandEncoder::BeginCopyPass()
    {
        return new DX12CopyPassCommandEncoder(device, *this, commandBuffer);
    }

    ComputePassCommandEncoder* DX12CommandEncoder::BeginComputePass()
    {
        return new DX12ComputePassCommandEncoder(device, *this, commandBuffer);
    }

    GraphicsPassCommandEncoder* DX12CommandEncoder::BeginGraphicsPass(const GraphicsPassBeginInfo& inBeginInfo)
    {
        return new DX12GraphicsPassCommandEncoder(device, *this, commandBuffer, inBeginInfo);
    }

    void DX12CommandEncoder::End()
    {
        commandBuffer.GetNative()->Close();
    }

    void DX12CommandEncoder::Destroy()
    {
        delete this;
    }
}
