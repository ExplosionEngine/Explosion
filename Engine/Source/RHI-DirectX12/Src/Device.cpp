//
// Created by johnk on 15/1/2022.
//

#include <iostream>
#include <unordered_set>

#include <directx/d3d12sdklayers.h>

#include <RHI/DirectX12/Device.h>
#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Instance.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Queue.h>
#include <RHI/DirectX12/Buffer.h>
#include <RHI/DirectX12/Texture.h>
#include <RHI/DirectX12/Sampler.h>
#include <RHI/DirectX12/BindGroupLayout.h>
#include <RHI/DirectX12/BindGroup.h>
#include <RHI/DirectX12/PipelineLayout.h>
#include <RHI/DirectX12/ShaderModule.h>
#include <RHI/DirectX12/Pipeline.h>
#include <RHI/DirectX12/CommandBuffer.h>
#include <RHI/DirectX12/SwapChain.h>
#include <RHI/DirectX12/Synchronous.h>
#include <RHI/DirectX12/Surface.h>
#include <Core/Log.h>

namespace RHI::DirectX12 {
    DescriptorAllocation::DescriptorAllocation(DescriptorHeapNode* inNode, const uint32_t inSlot, const CD3DX12_CPU_DESCRIPTOR_HANDLE& inHandle, ID3D12DescriptorHeap* inHeap)
        : node(inNode)
        , slot(inSlot)
        , cpuHandle(inHandle)
        , nativeDescriptorHeap(inHeap)
    {
    }

    DescriptorAllocation::~DescriptorAllocation()
    {
        node->Free(slot);
    }

    const CD3DX12_CPU_DESCRIPTOR_HANDLE& DescriptorAllocation::GetCpuHandle() const
    {
        return cpuHandle;
    }

    ID3D12DescriptorHeap* DescriptorAllocation::GetNativeDescriptorHeap() const
    {
        return nativeDescriptorHeap;
    }

    DescriptorHeapNode::DescriptorHeapNode(ComPtr<ID3D12DescriptorHeap>&& inHeap, uint32_t inDescriptorSize, uint32_t inCapacity)
        : nativeDescriptorHeap(std::move(inHeap))
        , descriptorSize(inDescriptorSize)
        , capacity(inCapacity)
        , free(inCapacity)
    {
        usedBitMasks.resize(capacity / 8, 0);
    }

    bool DescriptorHeapNode::HasFreeSlot() const
    {
        return free > 0;
    }

    Common::UniquePtr<DescriptorAllocation> DescriptorHeapNode::Allocate()
    {
        Assert(HasFreeSlot());

        std::optional<uint32_t> selectedSlot;
        for (auto slot = 0; slot < capacity; slot++) {
            const auto groupIndex = slot / 8;
            auto& bitMask = usedBitMasks[groupIndex];

            const auto indexInGroup = slot % 8;
            const auto slotMask = 1 << indexInGroup;

            if ((bitMask & slotMask) != 0) {
                continue;
            }

            bitMask = bitMask | slotMask;
            selectedSlot = slot;
            break;
        }

        Assert(selectedSlot.has_value());
        free--;
        Assert(free <= capacity);

        CD3DX12_CPU_DESCRIPTOR_HANDLE baseCpuHandle(nativeDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
        return Common::MakeUnique<DescriptorAllocation>(this, selectedSlot.value(), baseCpuHandle.Offset(selectedSlot.value(), descriptorSize), nativeDescriptorHeap.Get());
    }

    void DescriptorHeapNode::Free(uint32_t slot)
    {
        const auto groupIndex = slot / 8;
        auto& bitMask = usedBitMasks[groupIndex];

        const auto indexInGroup = slot % 8;
        const auto slotMask = 1 << indexInGroup;
        const auto slotIsUsed = (bitMask & slotMask) == slotMask;
        bitMask = bitMask & ~slotMask;

        Assert(slotIsUsed);
        free++;
        Assert(free <= capacity);
    }

    DescriptorPool::DescriptorPool(DX12Device& inDevice, D3D12_DESCRIPTOR_HEAP_TYPE inNativeHeapType, uint32_t inDescriptorSize, uint32_t inCapacity)
        : device(inDevice)
        , nativeHeapType(inNativeHeapType)
        , descriptorSize(inDescriptorSize)
        , capacity(inCapacity)
        , heapNodes()
    {
    }

    Common::UniquePtr<DescriptorAllocation> DescriptorPool::Allocate()
    {
        for (auto& node : heapNodes) {
            if (node.HasFreeSlot()) {
                return node.Allocate();
            }
        }

        ComPtr<ID3D12DescriptorHeap> nativeDescriptorHeap;

        D3D12_DESCRIPTOR_HEAP_DESC desc {};
        desc.NumDescriptors = capacity;
        desc.Type = nativeHeapType;
        desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
        Assert(SUCCEEDED(device.GetNative()->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&nativeDescriptorHeap))));

        heapNodes.emplace_back(std::move(nativeDescriptorHeap), descriptorSize, capacity);
        return heapNodes.back().Allocate();
    }

    DX12Device::DX12Device(DX12Gpu& inGpu, const DeviceCreateInfo& inCreateInfo)
        : Device(inCreateInfo)
#if BUILD_CONFIG_DEBUG
        , debugLayerExceptionHandlerReady(false)
#endif
        , gpu(inGpu)
        , nativeRtvDescriptorSize(0)
        , nativeCbvSrvUavDescriptorSize(0)
        , nativeSamplerDescriptorSize(0)
        , nativeDsvDescriptorSize(0)
    {
        CreateNativeDevice();
        CreateNativeQueues(inCreateInfo);
        QueryNativeDescriptorSize();
        CreateDescriptorPools();
#if BUILD_CONFIG_DEBUG
        RegisterNativeDebugLayerExceptionHandler();
#endif
    }

    DX12Device::~DX12Device()
    {
#if BUILD_CONFIG_DEBUG
        UnregisterNativeDebugLayerExceptionHandler();
#endif
    }

    size_t DX12Device::GetQueueNum(QueueType inType)
    {
        return queues.at(inType).size();
    }

    DX12Gpu& DX12Device::GetGpu() const
    {
        return gpu;
    }

    Queue* DX12Device::GetQueue(const QueueType inType, const size_t inIndex)
    {
        const auto& queueArray = queues.at(inType);
        Assert(inIndex < queueArray.size());
        return queueArray[inIndex].Get();
    }

    Common::UniquePtr<Surface> DX12Device::CreateSurface(const SurfaceCreateInfo& inCreateInfo)
    {
        return { new DX12Surface(inCreateInfo) };
    }

    Common::UniquePtr<SwapChain> DX12Device::CreateSwapChain(const SwapChainCreateInfo& inCreateInfo)
    {
        return { new DX12SwapChain(*this, inCreateInfo) };
    }

    Common::UniquePtr<Buffer> DX12Device::CreateBuffer(const BufferCreateInfo& inCreateInfo)
    {
        return { new DX12Buffer(*this, inCreateInfo) };
    }

    Common::UniquePtr<Texture> DX12Device::CreateTexture(const TextureCreateInfo& inCreateInfo)
    {
        return { new DX12Texture(*this, inCreateInfo) };
    }

    Common::UniquePtr<Sampler> DX12Device::CreateSampler(const SamplerCreateInfo& inCreateInfo)
    {
        return { new DX12Sampler(*this, inCreateInfo) };
    }

    Common::UniquePtr<BindGroupLayout> DX12Device::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo)
    {
        return { new DX12BindGroupLayout(inCreateInfo) };
    }

    Common::UniquePtr<BindGroup> DX12Device::CreateBindGroup(const BindGroupCreateInfo& inCreateInfo)
    {
        return { new DX12BindGroup(inCreateInfo) };
    }

    Common::UniquePtr<PipelineLayout> DX12Device::CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo)
    {
        return { new DX12PipelineLayout(*this, inCreateInfo) };
    }

    Common::UniquePtr<ShaderModule> DX12Device::CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo)
    {
        return { new DX12ShaderModule(inCreateInfo) };
    }

    Common::UniquePtr<ComputePipeline> DX12Device::CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo)
    {
        return { new DX12ComputePipeline(*this, inCreateInfo) };
    }

    Common::UniquePtr<RasterPipeline> DX12Device::CreateRasterPipeline(const RasterPipelineCreateInfo& inCreateInfo)
    {
        return { new DX12RasterPipeline(*this, inCreateInfo) };
    }

    Common::UniquePtr<CommandBuffer> DX12Device::CreateCommandBuffer()
    {
        return { new DX12CommandBuffer(*this) };
    }

    Common::UniquePtr<Fence> DX12Device::CreateFence(const bool inInitAsSignaled)
    {
        return { new DX12Fence(*this, inInitAsSignaled) };
    }

    Common::UniquePtr<Semaphore> DX12Device::CreateSemaphore()
    {
        return { new DX12Semaphore(*this) };
    }

    bool DX12Device::CheckSwapChainFormatSupport(Surface* inSurface, PixelFormat inFormat)
    {
        static std::unordered_set supportedFormats = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm,
            // TODO HDR
        };
        return supportedFormats.contains(inFormat);
    }

    TextureSubResourceCopyFootprint DX12Device::GetTextureSubResourceCopyFootprint(const Texture& texture, const TextureSubResourceInfo& subResourceInfo)
    {
        const auto& dx12Texture = static_cast<const DX12Texture&>(texture);
        const auto createInfo = texture.GetCreateInfo();
        const auto nativeResourceDesc = dx12Texture.GetNative()->GetDesc();

        const size_t nativeSubResourceIndex = D3D12CalcSubresource(subResourceInfo.mipLevel, subResourceInfo.arrayLayer, 0, 1, 1);

        D3D12_PLACED_SUBRESOURCE_FOOTPRINT footprint;
        nativeDevice->GetCopyableFootprints(&nativeResourceDesc, nativeSubResourceIndex, 1, 0, &footprint, nullptr, nullptr, nullptr);

        TextureSubResourceCopyFootprint result {};
        result.extent = { footprint.Footprint.Width, footprint.Footprint.Height, footprint.Footprint.Depth };
        result.bytesPerPixel = GetBytesPerPixel(createInfo.format);
        result.rowPitch = footprint.Footprint.RowPitch;
        result.slicePitch = footprint.Footprint.RowPitch * footprint.Footprint.Height;
        result.totalBytes = footprint.Footprint.RowPitch * footprint.Footprint.Height * footprint.Footprint.Depth;
        return result;
    }

    ID3D12Device* DX12Device::GetNative() const
    {
        return nativeDevice.Get();
    }

    Common::UniquePtr<DescriptorAllocation> DX12Device::AllocateRtvDescriptor() const
    {
        return rtvDescriptorPool->Allocate();
    }

    Common::UniquePtr<DescriptorAllocation> DX12Device::AllocateCbvSrvUavDescriptor() const
    {
        return cbvSrvUavDescriptorPool->Allocate();
    }

    Common::UniquePtr<DescriptorAllocation> DX12Device::AllocateSamplerDescriptor() const
    {
        return samplerDescriptorPool->Allocate();
    }

    Common::UniquePtr<DescriptorAllocation> DX12Device::AllocateDsvDescriptor() const
    {
        return dsvDescriptorPool->Allocate();
    }

    void DX12Device::CreateNativeDevice()
    {
        Assert(SUCCEEDED(D3D12CreateDevice(gpu.GetNative(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&nativeDevice))));
    }

    void DX12Device::CreateNativeQueues(const DeviceCreateInfo& inCreateInfo)
    {
        std::unordered_map<QueueType, size_t> queueNumMap;
        for (size_t i = 0; i < inCreateInfo.queueRequests.size(); i++) {
            const auto& queueCreateInfo = inCreateInfo.queueRequests[i];
            if (const auto iter = queueNumMap.find(queueCreateInfo.type);
                iter == queueNumMap.end()) {
                queueNumMap[queueCreateInfo.type] = 0;
            }
            queueNumMap[queueCreateInfo.type] += queueCreateInfo.num;
        }

        for (auto iter : queueNumMap) {
            std::vector<Common::UniquePtr<DX12Queue>> tempQueues(iter.second);

            D3D12_COMMAND_QUEUE_DESC queueDesc {};
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Type = EnumCast<QueueType, D3D12_COMMAND_LIST_TYPE>(iter.first);
            for (auto& j : tempQueues) {
                ComPtr<ID3D12CommandQueue> commandQueue;
                Assert(SUCCEEDED(nativeDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue))));
                j = Common::MakeUnique<DX12Queue>(std::move(commandQueue));
            }

            queues[iter.first] = std::move(tempQueues);
        }
    }

    void DX12Device::QueryNativeDescriptorSize()
    {
        nativeRtvDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        nativeCbvSrvUavDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        nativeSamplerDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        nativeDsvDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    void DX12Device::CreateDescriptorPools()
    {
        rtvDescriptorPool = Common::MakeUnique<DescriptorPool>(*this, D3D12_DESCRIPTOR_HEAP_TYPE_RTV, nativeRtvDescriptorSize, 16);
        cbvSrvUavDescriptorPool = Common::MakeUnique<DescriptorPool>(*this, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, nativeCbvSrvUavDescriptorSize, 16);
        samplerDescriptorPool = Common::MakeUnique<DescriptorPool>(*this, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER, nativeSamplerDescriptorSize, 16);
        dsvDescriptorPool = Common::MakeUnique<DescriptorPool>(*this, D3D12_DESCRIPTOR_HEAP_TYPE_DSV, nativeDsvDescriptorSize, 16);
    }

#if BUILD_CONFIG_DEBUG
    void DX12Device::RegisterNativeDebugLayerExceptionHandler()
    {
        ComPtr<ID3D12InfoQueue> dx12InfoQueue;
        if (FAILED(nativeDevice->QueryInterface(IID_PPV_ARGS(&dx12InfoQueue)))) {
            LogError(RHI, "DX12 debug layer can not enabled, please check the graphics tools installed in windows.");
            return;
        }
        debugLayerExceptionHandlerReady = true;
        Assert(SUCCEEDED(dx12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true)));

        gpu.GetInstance().AddDebugLayerExceptionHandler(this, [this]() -> void {
            ComPtr<ID3D12Debug> dx12Debug;
            Assert(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dx12Debug))));

            ComPtr<ID3D12InfoQueue> dx12InfoQueueLocal;
            Assert(SUCCEEDED(nativeDevice->QueryInterface(IID_PPV_ARGS(&dx12InfoQueueLocal))));

            const auto messageCount = dx12InfoQueueLocal->GetNumStoredMessagesAllowedByRetrievalFilter();
            for (auto i = 0; i < messageCount; i++) {
                std::pair<D3D12_MESSAGE*, size_t> message;
                Assert(SUCCEEDED(dx12InfoQueueLocal->GetMessageA(i, nullptr, &message.second)));
                message.first = static_cast<D3D12_MESSAGE*>(malloc(message.second));
                Assert(SUCCEEDED(dx12InfoQueueLocal->GetMessageA(i, message.first, &message.second)));

                if (message.first->Severity == D3D12_MESSAGE_SEVERITY_ERROR) {
                    LogError(RHI, "{}", message.first->pDescription);
                }
                free(message.first);
            }
        });
    }

    void DX12Device::UnregisterNativeDebugLayerExceptionHandler()
    {
        if (!debugLayerExceptionHandlerReady) {
            return;
        }
        gpu.GetInstance().RemoveDebugLayerExceptionHandler(this);
        debugLayerExceptionHandlerReady = false;
    }
#endif
}
