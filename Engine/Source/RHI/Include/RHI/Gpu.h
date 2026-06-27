//
// Created by johnk on 12/1/2022.
//

#pragma once

#include <cstdint>

#include <Common/Utility.h>
#include <RHI/Common.h>

namespace RHI {
    class Device;
    class Instance;
    struct DeviceCreateInfo;

    struct GpuProperty {
        uint32_t vendorId;
        uint32_t deviceId;
        GpuType type;
    };

    struct GpuLimits {
        uint32_t maxTextureDimension1D;
        uint32_t maxTextureDimension2D;
        uint32_t maxTextureDimension3D;
        uint32_t maxTextureArrayLayers;
        uint32_t maxBindGroups;
        uint32_t maxVertexBuffers;
        uint32_t maxVertexAttributes;
        uint32_t maxColorAttachments;
        uint64_t maxUniformBufferBindingSize;
        uint64_t maxStorageBufferBindingSize;
        uint64_t maxBufferSize;
        uint32_t minUniformBufferOffsetAlignment;
        uint32_t minStorageBufferOffsetAlignment;
        uint32_t optimalBufferCopyOffsetAlignment;
        uint32_t optimalBufferCopyRowPitchAlignment;
        uint32_t maxComputeWorkgroupSizeX;
        uint32_t maxComputeWorkgroupSizeY;
        uint32_t maxComputeWorkgroupSizeZ;
        uint32_t maxComputeInvocationsPerWorkgroup;
        uint32_t maxComputeWorkgroupsPerDimension;
    };

    class Gpu {
    public:
        NonCopyable(Gpu)
        virtual ~Gpu();
        virtual GpuProperty GetProperty() = 0;
        virtual FeatureFlags GetFeatures() = 0;
        virtual GpuLimits GetLimits() = 0;
        virtual Common::UniquePtr<Device> RequestDevice(const DeviceCreateInfo& createInfo) = 0;
        virtual Instance& GetInstance() const = 0;

    protected:
        Gpu();
    };
}
