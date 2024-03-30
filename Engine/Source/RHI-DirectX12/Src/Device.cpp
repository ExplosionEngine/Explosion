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

namespace RHI::DirectX12 {
    DX12Device::DX12Device(DX12Gpu& inGpu, const DeviceCreateInfo& inCreateInfo) : Device(inCreateInfo), gpu(inGpu), nativeRtvDescriptorSize(0), nativeCbvSrvUavDescriptorSize(0)
    {
        CreateNativeDevice();
        CreateNativeQueues(inCreateInfo);
        CreateNativeCmdAllocator();
        QueryNativeDescriptorSize();
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

    void DX12Device::Destroy()
    {
        delete this;
    }

    size_t DX12Device::GetQueueNum(QueueType inType)
    {
        auto iter = queues.find(inType);
        Assert(iter != queues.end());
        return iter->second.size();
    }

    Queue* DX12Device::GetQueue(QueueType inType, size_t inIndex)
    {
        auto iter = queues.find(inType);
        Assert(iter != queues.end());
        auto& queueArray = iter->second;
        Assert(inIndex >= 0 && inIndex < queueArray.size());
        return queueArray[inIndex].Get();
    }

    Surface* DX12Device::CreateSurface(const SurfaceCreateInfo& inCreateInfo)
    {
        return new DX12Surface(inCreateInfo);
    }

    SwapChain* DX12Device::CreateSwapChain(const SwapChainCreateInfo& inCreateInfo)
    {
        return new DX12SwapChain(*this, inCreateInfo);
    }

    ID3D12CommandAllocator* DX12Device::GetNativeCmdAllocator()
    {
        return nativeCmdAllocator.Get();
    }

    Buffer* DX12Device::CreateBuffer(const BufferCreateInfo& inCreateInfo)
    {
        return new DX12Buffer(*this, inCreateInfo);
    }

    Texture* DX12Device::CreateTexture(const TextureCreateInfo& inCreateInfo)
    {
        return new DX12Texture(*this, inCreateInfo);
    }

    Sampler* DX12Device::CreateSampler(const SamplerCreateInfo& inCreateInfo)
    {
        return new DX12Sampler(*this, inCreateInfo);
    }

    BindGroupLayout* DX12Device::CreateBindGroupLayout(const BindGroupLayoutCreateInfo& inCreateInfo)
    {
        return new DX12BindGroupLayout(inCreateInfo);
    }

    BindGroup* DX12Device::CreateBindGroup(const BindGroupCreateInfo& inCreateInfo)
    {
        return new DX12BindGroup(inCreateInfo);
    }

    PipelineLayout* DX12Device::CreatePipelineLayout(const PipelineLayoutCreateInfo& inCreateInfo)
    {
        return new DX12PipelineLayout(*this, inCreateInfo);
    }

    ShaderModule* DX12Device::CreateShaderModule(const ShaderModuleCreateInfo& inCreateInfo)
    {
        return new DX12ShaderModule(inCreateInfo);
    }

    ComputePipeline* DX12Device::CreateComputePipeline(const ComputePipelineCreateInfo& inCreateInfo)
    {
        return new DX12ComputePipeline(*this, inCreateInfo);
    }

    GraphicsPipeline* DX12Device::CreateGraphicsPipeline(const GraphicsPipelineCreateInfo& inCreateInfo)
    {
        return new DX12GraphicsPipeline(*this, inCreateInfo);
    }

    CommandBuffer* DX12Device::CreateCommandBuffer()
    {
        return new DX12CommandBuffer(*this);
    }

    Fence* DX12Device::CreateFence(bool inInitAsSignaled)
    {
        return new DX12Fence(*this, inInitAsSignaled);
    }

    Semaphore* DX12Device::CreateSemaphore()
    {
        return new DX12Semaphore(*this);
    }

    bool DX12Device::CheckSwapChainFormatSupport(RHI::Surface* inSurface, PixelFormat inFormat)
    {
        static std::unordered_set<PixelFormat> supportedFormats = {
            PixelFormat::rgba8Unorm,
            PixelFormat::bgra8Unorm,
            // TODO HDR
        };
        return supportedFormats.contains(inFormat);
    }

    DX12Gpu& DX12Device::GetGpu()
    {
        return gpu;
    }

    ID3D12Device* DX12Device::GetNative()
    {
        return nativeDevice.Get();
    }

    NativeDescriptorAllocation DX12Device::AllocateNativeRtvDescriptor()
    {
        return AllocateNativeDescriptor(nativeRtvHeapList, 4, nativeRtvDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
    }

    NativeDescriptorAllocation DX12Device::AllocateNativeCbvSrvUavDescriptor()
    {
        return AllocateNativeDescriptor(nativeCbvSrvUavHeapList, 4, nativeCbvSrvUavDescriptorSize,
                                        D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
    }

    NativeDescriptorAllocation DX12Device::AllocateNativeSamplerDescriptor()
    {
        return AllocateNativeDescriptor(nativeSamplerHeapList, 4, nativeSamplerDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
    }

    NativeDescriptorAllocation DX12Device::AllocateNativeDsvDescriptor()
    {
        return AllocateNativeDescriptor(nativeDsvHeapList, 1, nativeDsvDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

    NativeDescriptorAllocation DX12Device::AllocateNativeDescriptor(std::list<NativeDescriptorHeapListNode>& inList, uint8_t inCapacity, uint32_t inDescriptorSize, D3D12_DESCRIPTOR_HEAP_TYPE inHeapType)
    {
        if (inList.empty() || inList.back().used >= inCapacity) {
            NativeDescriptorHeapListNode node {};

            D3D12_DESCRIPTOR_HEAP_DESC desc {};
            desc.NumDescriptors = inCapacity;
            desc.Type = inHeapType;
            desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
            bool success = SUCCEEDED(nativeDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&node.descriptorHeap)));
            Assert(success);
            inList.emplace_back(std::move(node));
        }

        auto& last = inList.back();

        CD3DX12_CPU_DESCRIPTOR_HANDLE cpuHandle(last.descriptorHeap->GetCPUDescriptorHandleForHeapStart());
        auto offset = last.used++;
        return {
            cpuHandle.Offset(offset, inDescriptorSize),
            last.descriptorHeap.Get()
        };
    }

    void DX12Device::CreateNativeDevice()
    {
        bool success = SUCCEEDED(D3D12CreateDevice(gpu.GetNative(), D3D_FEATURE_LEVEL_12_1, IID_PPV_ARGS(&nativeDevice)));
        Assert(success);
    }

    void DX12Device::CreateNativeQueues(const DeviceCreateInfo& inCreateInfo)
    {
        std::unordered_map<QueueType, size_t> queueNumMap;
        for (size_t i = 0; i < inCreateInfo.queueRequests.size(); i++) {
            const auto& queueCreateInfo = inCreateInfo.queueRequests[i];
            auto iter = queueNumMap.find(queueCreateInfo.type);
            if (iter == queueNumMap.end()) {
                queueNumMap[queueCreateInfo.type] = 0;
            }
            queueNumMap[queueCreateInfo.type] += queueCreateInfo.num;
        }

        for (auto iter : queueNumMap) {
            std::vector<Common::UniqueRef<DX12Queue>> tempQueues(iter.second);

            D3D12_COMMAND_QUEUE_DESC queueDesc {};
            queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
            queueDesc.Type = DX12EnumCast<QueueType, D3D12_COMMAND_LIST_TYPE>(iter.first);
            for (auto& j : tempQueues) {
                ComPtr<ID3D12CommandQueue> commandQueue;
                nativeDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&commandQueue));
                Assert(commandQueue);
                j = Common::MakeUnique<DX12Queue>(std::move(commandQueue));
            }

            queues[iter.first] = std::move(tempQueues);
        }
    }

    void DX12Device::CreateNativeCmdAllocator()
    {
        nativeDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&nativeCmdAllocator));
    }

    void DX12Device::QueryNativeDescriptorSize()
    {
        nativeRtvDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
        nativeCbvSrvUavDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
        nativeSamplerDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER);
        nativeDsvDescriptorSize = nativeDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
    }

#if BUILD_CONFIG_DEBUG
    void DX12Device::RegisterNativeDebugLayerExceptionHandler()
    {
        ComPtr<ID3D12InfoQueue> dx12InfoQueue;
        Assert(SUCCEEDED(nativeDevice->QueryInterface(IID_PPV_ARGS(&dx12InfoQueue))));
        dx12InfoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);

        gpu.GetInstance().AddDebugLayerExceptionHandler(this, [this]() -> void {
            ComPtr<ID3D12Debug> dx12Debug;
            Assert(SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&dx12Debug))));

            ComPtr<ID3D12InfoQueue> dx12InfoQueue;
            Assert(SUCCEEDED(nativeDevice->QueryInterface(IID_PPV_ARGS(&dx12InfoQueue))));

            auto messageCount = dx12InfoQueue->GetNumStoredMessagesAllowedByRetrievalFilter();
            for (auto i = 0; i < messageCount; i++) {
                std::pair<D3D12_MESSAGE*, size_t> message;
                Assert(SUCCEEDED(dx12InfoQueue->GetMessageA(i, nullptr, &message.second)));
                message.first = static_cast<D3D12_MESSAGE*>(malloc(message.second));
                Assert(SUCCEEDED(dx12InfoQueue->GetMessageA(i, message.first, &message.second)));

                if (message.first->Severity == D3D12_MESSAGE_SEVERITY_ERROR) {
                    std::cout << message.first->pDescription << std::endl;
                }
                free(message.first);
            }
        });
    }

    void DX12Device::UnregisterNativeDebugLayerExceptionHandler()
    {
        gpu.GetInstance().RemoveDebugLayerExceptionHandler(this);
    }
#endif
}
