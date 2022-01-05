//
// Created by johnk on 4/1/2022.
//

#ifndef EXPLOSION_RHI_DX12_BUFFER_H
#define EXPLOSION_RHI_DX12_BUFFER_H

#include <RHI/Buffer.h>

namespace RHI::DirectX12 {
    class DX12DeviceMemory;

    class DX12Buffer : public Buffer {
    public:
        NON_COPYABLE(DX12Buffer)
        explicit DX12Buffer(const BufferCreateInfo* createInfo);
        ~DX12Buffer() override;

        void BindMemory(DX12DeviceMemory* deviceMemory);
        DX12DeviceMemory* GetDeviceMemory();

    private:
        DX12DeviceMemory* deviceMemory;
        size_t size;
        BufferUsageFlags usage;
    };
}

#endif //EXPLOSION_RHI_DX12_BUFFER_H
