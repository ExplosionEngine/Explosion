//
// Created by johnk on 15/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_DEVICE_H
#define EXPLOSION_RHI_DX12_DEVICE_H

#include <vector>
#include <memory>
#include <unordered_map>

#include <wrl/client.h>
#include <d3d12.h>

#include <RHI/Device.h>

using namespace Microsoft::WRL;

namespace RHI::DirectX12 {
    class DX12Gpu;
    class DX12Queue;

    class DX12Device : public Device {
    public:
        NON_COPYABLE(DX12Device)
        DX12Device(DX12Gpu& gpu, const DeviceCreateInfo* createInfo);
        ~DX12Device() override;

        void Destroy() override;
        size_t GetQueueNum(QueueType type) override;
        Queue* GetQueue(QueueType type, size_t index) override;
        Buffer* CreateBuffer(const BufferCreateInfo* createInfo) override;
        Texture* CreateTexture(const TextureCreateInfo* createInfo) override;
        Sampler* CreateSampler(const SamplerCreateInfo* createInfo) override;

        ComPtr<ID3D12Device>& GetDX12Device();

    private:
        void CreateDevice(DX12Gpu& gpu);
        void CreateQueues(const DeviceCreateInfo* createInfo);

        ComPtr<ID3D12Device> dx12Device;
        std::unordered_map<QueueType, std::vector<std::unique_ptr<DX12Queue>>> queues;
    };
}

#endif //EXPLOSION_RHI_DX12_DEVICE_H
