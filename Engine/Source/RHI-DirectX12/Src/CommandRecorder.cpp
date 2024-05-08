//
// Created by johnk on 2022/3/23.
//

#include <optional>
#include <array>

#include <RHI/DirectX12/CommandRecorder.h>
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
    static D3D12_CLEAR_FLAGS GetDX12ClearFlags(const DepthStencilAttachment& depthStencilAttachment)
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

    size_t GetNativeSubResourceIndex(const DX12Texture& texture, const TextureSubResourceInfo& subResource)
    {
        const auto& createInfo = texture.GetCreateInfo();
        return D3D12CalcSubresource(subResource.mipLevel, subResource.arrayLayer, 0, createInfo.mipLevels, createInfo.dimension == TextureDimension::t3D ? 1 : createInfo.depthOrArraySize);
    }

    CD3DX12_TEXTURE_COPY_LOCATION GetNativeTextureCopyLocation(const DX12Texture& texture, const TextureSubResourceInfo& subResource)
    {
        return CD3DX12_TEXTURE_COPY_LOCATION(texture.GetNative(), GetNativeSubResourceIndex(texture, subResource));
    }

    size_t GetNativeBytesPerPixel(DX12Device& device, const DX12Texture& texture, const TextureSubResourceInfo& subResource)
    {
        const auto nativeResourceDesc = texture.GetNative()->GetDesc();

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        device.GetNative()->GetCopyableFootprints(&nativeResourceDesc, GetNativeSubResourceIndex(texture, subResource), 1, 0, &footprint, nullptr, nullptr, nullptr);
        return footprint.Footprint.RowPitch / footprint.Footprint.Width;
    }
}

namespace RHI::DirectX12 {
    DX12CopyPassCommandRecorder::DX12CopyPassCommandRecorder(DX12Device& inDevice, DX12CommandRecorder& inCmdRecorder, DX12CommandBuffer& inCmdBuffer)
        : device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
    {
    }

    DX12CopyPassCommandRecorder::~DX12CopyPassCommandRecorder() = default;

    void DX12CopyPassCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandRecorder.ResourceBarrier(inBarrier);
    }

    void DX12CopyPassCommandRecorder::CopyBufferToBuffer(Buffer* src, Buffer* dst, const BufferCopyInfo& copyInfo)
    {
        auto* srcBuffer = static_cast<DX12Buffer*>(src);
        auto* dstBuffer = static_cast<DX12Buffer*>(dst);

        commandBuffer.GetNative()->CopyBufferRegion(
            dstBuffer->GetNative(), copyInfo.dstOffset,
            srcBuffer->GetNative(), copyInfo.srcOffset,
            copyInfo.copySize);
    }

    void DX12CopyPassCommandRecorder::CopyBufferToTexture(Buffer* src, Texture* dst, const BufferTextureCopyInfo& copyInfo)
    {
        auto* srcBuffer = static_cast<DX12Buffer*>(src);
        auto* dstTexture = static_cast<DX12Texture*>(dst);

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT srcLayout;
        srcLayout.Offset = copyInfo.bufferOffset;
        srcLayout.Footprint.Format = dstTexture->GetNative()->GetDesc().Format;
        srcLayout.Footprint.Width = copyInfo.copyRegion.x;
        srcLayout.Footprint.Height = copyInfo.copyRegion.y;
        srcLayout.Footprint.Depth = copyInfo.copyRegion.z;
        srcLayout.Footprint.RowPitch = srcLayout.Footprint.Width * GetNativeBytesPerPixel(device, *dstTexture, copyInfo.textureSubResource);

        const CD3DX12_TEXTURE_COPY_LOCATION srcCopyRegion(srcBuffer->GetNative(), srcLayout);
        const CD3DX12_TEXTURE_COPY_LOCATION dstCopyRegion = GetNativeTextureCopyLocation(*dstTexture, copyInfo.textureSubResource);

        D3D12_BOX srcBox;
        srcBox.left = 0;
        srcBox.right = copyInfo.copyRegion.x;
        srcBox.top = 0;
        srcBox.bottom = copyInfo.copyRegion.y;
        srcBox.front = 0;
        srcBox.back = copyInfo.copyRegion.z;

        commandBuffer.GetNative()->CopyTextureRegion(
            &dstCopyRegion,
            copyInfo.textureOrigin.x,
            copyInfo.textureOrigin.y,
            copyInfo.textureOrigin.z,
            &srcCopyRegion,
            &srcBox);
    }

    void DX12CopyPassCommandRecorder::CopyTextureToBuffer(Texture* src, Buffer* dst, const BufferTextureCopyInfo& copyInfo)
    {
        // TODO
    }

    void DX12CopyPassCommandRecorder::CopyTextureToTexture(Texture* src, Texture* dst, const TextureCopyInfo& copyInfo)
    {
        auto* srcTexture = static_cast<DX12Texture*>(src);
        auto* dstTexture = static_cast<DX12Texture*>(dst);

        D3D12_BOX srcBox;
        srcBox.left = copyInfo.srcOrigin.x;
        srcBox.right = copyInfo.srcOrigin.x + copyInfo.copyRegion.x;
        srcBox.top = copyInfo.srcOrigin.y;
        srcBox.bottom = copyInfo.srcOrigin.y + copyInfo.copyRegion.y;
        srcBox.front = copyInfo.srcOrigin.z;
        srcBox.back = copyInfo.srcOrigin.z + copyInfo.copyRegion.z;

        CD3DX12_TEXTURE_COPY_LOCATION srcCopyRegion = GetNativeTextureCopyLocation(*srcTexture, copyInfo.srcSubResource);
        CD3DX12_TEXTURE_COPY_LOCATION dstCopyRegion = GetNativeTextureCopyLocation(*dstTexture, copyInfo.dstSubResource);

        commandBuffer.GetNative()->CopyTextureRegion(
            &dstCopyRegion,
            copyInfo.dstOrigin.x,
            copyInfo.dstOrigin.y,
            copyInfo.dstOrigin.z,
            &srcCopyRegion,
            &srcBox);
    }

    void DX12CopyPassCommandRecorder::EndPass()
    {
    }

    DX12ComputePassCommandRecorder::DX12ComputePassCommandRecorder(DX12Device& inDevice, DX12CommandRecorder& inCmdRecorder, DX12CommandBuffer& inCmdBuffer)
        : ComputePassCommandRecorder()
        , device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
    {
    }

    DX12ComputePassCommandRecorder::~DX12ComputePassCommandRecorder() = default;

    void DX12ComputePassCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandRecorder.ResourceBarrier(inBarrier);
    }

    void DX12ComputePassCommandRecorder::SetPipeline(ComputePipeline* inPipeline)
    {
        computePipeline = static_cast<DX12ComputePipeline*>(inPipeline);
        Assert(computePipeline);

        commandBuffer.GetNative()->SetPipelineState(computePipeline->GetNative());
        commandBuffer.GetNative()->SetGraphicsRootSignature(computePipeline->GetPipelineLayout().GetNative());
    }

    void DX12ComputePassCommandRecorder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
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

    void DX12ComputePassCommandRecorder::Dispatch(size_t inGroupCountX, size_t inGroupCountY, size_t inGroupCountZ)
    {
        commandBuffer.GetNative()->Dispatch(inGroupCountX, inGroupCountY, inGroupCountZ);
    }

    void DX12ComputePassCommandRecorder::EndPass()
    {
    }

    DX12RasterPassCommandRecorder::DX12RasterPassCommandRecorder(DX12Device& inDevice, DX12CommandRecorder& inCmdRecorder, DX12CommandBuffer& inCmdBuffer, const RasterPassBeginInfo& inBeginInfo)
        : RasterPassCommandRecorder()
        , device(inDevice)
        , commandRecorder(inCmdRecorder)
        , commandBuffer(inCmdBuffer)
        , rasterPipeline(nullptr)
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

    DX12RasterPassCommandRecorder::~DX12RasterPassCommandRecorder() = default;

    void DX12RasterPassCommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        commandRecorder.ResourceBarrier(inBarrier);
    }

    void DX12RasterPassCommandRecorder::SetPipeline(RasterPipeline* inPipeline)
    {
        rasterPipeline = static_cast<DX12RasterPipeline*>(inPipeline);
        Assert(rasterPipeline);

        commandBuffer.GetNative()->SetPipelineState(rasterPipeline->GetNative());
        commandBuffer.GetNative()->SetGraphicsRootSignature(rasterPipeline->GetPipelineLayout().GetNative());
    }

    void DX12RasterPassCommandRecorder::SetBindGroup(uint8_t inLayoutIndex, BindGroup* inBindGroup)
    {
        auto* bindGroup = static_cast<DX12BindGroup*>(inBindGroup);
        auto& bindGroupLayout = bindGroup->GetBindGroupLayout();
        auto& pipelineLayout = rasterPipeline->GetPipelineLayout();

        Assert(inLayoutIndex == bindGroupLayout.GetLayoutIndex());
        Assert(rasterPipeline);

        const auto& bindings= bindGroup->GetNativeBindings();
        for (const auto& binding : bindings) {
            std::optional<BindingTypeAndRootParameterIndex> t = pipelineLayout.QueryRootDescriptorParameterIndex(inLayoutIndex, binding.first);
            if (!t.has_value()) {
                return;
            }
            commandBuffer.GetNative()->SetGraphicsRootDescriptorTable(t.value().second, commandBuffer.GetRuntimeDescriptorHeaps()->NewGpuDescriptorHandle(binding.first.rangeType, binding.second));
        }
    }

    void DX12RasterPassCommandRecorder::SetIndexBuffer(BufferView* inBufferView)
    {
        auto* bufferView = static_cast<DX12BufferView*>(inBufferView);
        commandBuffer.GetNative()->IASetIndexBuffer(&bufferView->GetNativeIndexBufferView());
    }

    void DX12RasterPassCommandRecorder::SetVertexBuffer(size_t inSlot, BufferView* inBufferView)
    {
        auto* bufferView = static_cast<DX12BufferView*>(inBufferView);
        commandBuffer.GetNative()->IASetVertexBuffers(inSlot, 1, &bufferView->GetNativeVertexBufferView());
    }

    void DX12RasterPassCommandRecorder::Draw(size_t inVertexCount, size_t inInstanceCount, size_t inFirstVertex, size_t inFirstInstance)
    {
        commandBuffer.GetNative()->DrawInstanced(inVertexCount, inInstanceCount, inFirstVertex, inFirstInstance);
    }

    void DX12RasterPassCommandRecorder::DrawIndexed(size_t inIndexCount, size_t inInstanceCount, size_t inFirstIndex, size_t inBaseVertex, size_t inFirstInstance)
    {
        commandBuffer.GetNative()->DrawIndexedInstanced(inIndexCount, inInstanceCount, inFirstIndex, inBaseVertex, inFirstInstance);
    }

    void DX12RasterPassCommandRecorder::SetViewport(float inX, float inY, float inWidth, float inHeight, float inMinDepth, float inMaxDepth)
    {
        // (x, y) = topLeft
        CD3DX12_VIEWPORT viewport = CD3DX12_VIEWPORT(inX, inY, inWidth, inHeight, inMinDepth, inMaxDepth);
        commandBuffer.GetNative()->RSSetViewports(1, &viewport);
    }

    void DX12RasterPassCommandRecorder::SetScissor(uint32_t inLeft, uint32_t inTop, uint32_t inRight, uint32_t inBottom)
    {
        CD3DX12_RECT scissor = CD3DX12_RECT(inLeft, inTop, inRight, inBottom);
        commandBuffer.GetNative()->RSSetScissorRects(1, &scissor);
    }

    void DX12RasterPassCommandRecorder::SetBlendConstant(const float* inConstants)
    {
        commandBuffer.GetNative()->OMSetBlendFactor(inConstants);
    }

    void DX12RasterPassCommandRecorder::SetPrimitiveTopology(PrimitiveTopology inPrimitiveTopology)
    {
        commandBuffer.GetNative()->IASetPrimitiveTopology(EnumCast<PrimitiveTopology, D3D_PRIMITIVE_TOPOLOGY>(inPrimitiveTopology));
    }

    void DX12RasterPassCommandRecorder::SetStencilReference(uint32_t inReference)
    {
        commandBuffer.GetNative()->OMSetStencilRef(inReference);
    }

    void DX12RasterPassCommandRecorder::EndPass()
    {
    }

    DX12CommandRecorder::DX12CommandRecorder(DX12Device& inDevice, DX12CommandBuffer& inCmdBuffer) : CommandRecorder(), device(inDevice), commandBuffer(inCmdBuffer)
    {
        inCmdBuffer.GetNative()->Reset(inDevice.GetNativeCmdAllocator(), nullptr);

        inCmdBuffer.GetRuntimeDescriptorHeaps()->ResetUsed();
        auto activeHeap = inCmdBuffer.GetRuntimeDescriptorHeaps()->GetNative();
        inCmdBuffer.GetNative()->SetDescriptorHeaps(activeHeap.size(), activeHeap.data());
    }

    DX12CommandRecorder::~DX12CommandRecorder() = default;

    void DX12CommandRecorder::ResourceBarrier(const Barrier& inBarrier)
    {
        ID3D12Resource* resource;
        D3D12_RESOURCE_STATES beforeState;
        D3D12_RESOURCE_STATES afterState;
        if (inBarrier.type == ResourceType::buffer) {
            auto* buffer = static_cast<DX12Buffer*>(inBarrier.buffer.pointer);
            Assert(buffer);
            resource = buffer->GetNative();
            beforeState = EnumCast<BufferState, D3D12_RESOURCE_STATES>(inBarrier.buffer.before);
            afterState = EnumCast<BufferState, D3D12_RESOURCE_STATES>(inBarrier.buffer.after);
        } else {
            auto* texture = static_cast<DX12Texture*>(inBarrier.texture.pointer);
            Assert(texture);
            resource = texture->GetNative();
            beforeState = EnumCast<TextureState, D3D12_RESOURCE_STATES>(inBarrier.texture.before);
            afterState = EnumCast<TextureState, D3D12_RESOURCE_STATES>(inBarrier.texture.after);
        }

        CD3DX12_RESOURCE_BARRIER resourceBarrier = CD3DX12_RESOURCE_BARRIER::Transition(resource, beforeState, afterState);
        commandBuffer.GetNative()->ResourceBarrier(1, &resourceBarrier);
    }

    Common::UniqueRef<CopyPassCommandRecorder> DX12CommandRecorder::BeginCopyPass()
    {
        return Common::UniqueRef<CopyPassCommandRecorder>(new DX12CopyPassCommandRecorder(device, *this, commandBuffer));
    }

    Common::UniqueRef<ComputePassCommandRecorder> DX12CommandRecorder::BeginComputePass()
    {
        return Common::UniqueRef<ComputePassCommandRecorder>(new DX12ComputePassCommandRecorder(device, *this, commandBuffer));
    }

    Common::UniqueRef<RasterPassCommandRecorder> DX12CommandRecorder::BeginRasterPass(const RasterPassBeginInfo& inBeginInfo)
    {
        return Common::UniqueRef<RasterPassCommandRecorder>(new DX12RasterPassCommandRecorder(device, *this, commandBuffer, inBeginInfo));
    }

    void DX12CommandRecorder::End()
    {
        commandBuffer.GetNative()->Close();
    }
}
