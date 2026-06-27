//
// Created by johnk on 13/1/2022.
//

#include <RHI/DirectX12/Common.h>
#include <RHI/DirectX12/Gpu.h>
#include <RHI/DirectX12/Device.h>

namespace RHI::DirectX12 {
    DX12Gpu::DX12Gpu(DX12Instance& inInstance, ComPtr<IDXGIAdapter1>&& inNativeAdapter)
        : instance(inInstance)
        , nativeAdapter(inNativeAdapter)
    {
    }

    DX12Gpu::~DX12Gpu() = default;

    GpuProperty DX12Gpu::GetProperty()
    {
        DXGI_ADAPTER_DESC1 desc;
        Assert(SUCCEEDED(nativeAdapter->GetDesc1(&desc)));

        GpuProperty property {};
        property.vendorId = desc.VendorId;
        property.deviceId = desc.DeviceId;
        property.type = desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE ? GpuType::software : GpuType::hardware;
        return property;
    }

    FeatureFlags DX12Gpu::GetFeatures()
    {
        return FeatureBits::samplerAnisotropy
            | FeatureBits::textureCompressionBc
            | FeatureBits::timestampQuery
            | FeatureBits::multiDrawIndirect
            | FeatureBits::drawIndirectFirstInstance;
    }

    GpuLimits DX12Gpu::GetLimits()
    {
        // D3D12 has no bind-group concept and its resource-size cap is tier-dependent, so both use conservative values.
        constexpr uint32_t maxBindGroups = 8;
        constexpr uint64_t maxResourceSize = 1ull << 31;

        GpuLimits result {};
        result.maxTextureDimension1D = D3D12_REQ_TEXTURE1D_U_DIMENSION;
        result.maxTextureDimension2D = D3D12_REQ_TEXTURE2D_U_OR_V_DIMENSION;
        result.maxTextureDimension3D = D3D12_REQ_TEXTURE3D_U_V_OR_W_DIMENSION;
        result.maxTextureArrayLayers = D3D12_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION;
        result.maxBindGroups = maxBindGroups;
        result.maxVertexBuffers = D3D12_IA_VERTEX_INPUT_RESOURCE_SLOT_COUNT;
        result.maxVertexAttributes = D3D12_VS_INPUT_REGISTER_COUNT;
        result.maxColorAttachments = D3D12_SIMULTANEOUS_RENDER_TARGET_COUNT;
        result.maxUniformBufferBindingSize = static_cast<uint64_t>(D3D12_REQ_CONSTANT_BUFFER_ELEMENT_COUNT) * 16;
        result.maxStorageBufferBindingSize = maxResourceSize;
        result.maxBufferSize = maxResourceSize;
        result.minUniformBufferOffsetAlignment = D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT;
        result.minStorageBufferOffsetAlignment = D3D12_RAW_UAV_SRV_BYTE_ALIGNMENT;
        result.optimalBufferCopyOffsetAlignment = D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT;
        result.optimalBufferCopyRowPitchAlignment = D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
        result.maxComputeWorkgroupSizeX = D3D12_CS_THREAD_GROUP_MAX_X;
        result.maxComputeWorkgroupSizeY = D3D12_CS_THREAD_GROUP_MAX_Y;
        result.maxComputeWorkgroupSizeZ = D3D12_CS_THREAD_GROUP_MAX_Z;
        result.maxComputeInvocationsPerWorkgroup = D3D12_CS_THREAD_GROUP_MAX_THREADS_PER_GROUP;
        result.maxComputeWorkgroupsPerDimension = D3D12_CS_DISPATCH_MAX_THREAD_GROUPS_PER_DIMENSION;
        return result;
    }

    DX12Instance& DX12Gpu::GetInstance() const
    {
        return instance;
    }

    IDXGIAdapter1* DX12Gpu::GetNative() const
    {
        return nativeAdapter.Get();
    }

    Common::UniquePtr<Device> DX12Gpu::RequestDevice(const DeviceCreateInfo& inCreateInfo)
    {
        return Common::UniquePtr<Device>(new DX12Device(*this, inCreateInfo));
    }
}
