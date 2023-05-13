//
// Created by johnk on 10/1/2022.
//

#pragma once

#include <stdexcept>
#include <utility>
#include <unordered_map>
#include <format>
#include <functional>

#include <dxgi1_4.h>
#include <d3d12.h>

#include <Common/Debug.h>
#include <RHI/Common.h>

// duplicated code because static variable and namespace
namespace RHI::DirectX12 {
    template <typename A, typename B>
    static const std::unordered_map<A, B> DX12_ENUM_MAP;

    template <typename A, typename B>
    B DX12EnumCast(const A& value)
    {
        auto iter = DX12_ENUM_MAP<A, B>.find(value);
        Assert((iter != DX12_ENUM_MAP<A, B>.end()));
        return static_cast<B>(iter->second);
    }

#define DX12_ENUM_MAP_BEGIN(A, B) template <> static const std::unordered_map<A, B> DX12_ENUM_MAP<A, B> = {
#define DX12_ENUM_MAP_ITEM(A, B) { A, B },
#define DX12_ENUM_MAP_END() };

    template <typename E>
    using BitsTypeForEachFunc = std::function<void(E e)>;

    template <typename E>
    void ForEachBitsType(BitsTypeForEachFunc<E>&& func)
    {
        using UBitsType = std::underlying_type_t<E>;
        for (UBitsType i = 0x1; i < static_cast<UBitsType>(E::max); i = i << 1) {
            func(static_cast<E>(i));
        }
    }
}

// hard code convert
namespace RHI::DirectX12 {
    GpuType GetGpuTypeByAdapterFlag(UINT flag);
}

// enum cast impl
namespace RHI::DirectX12 {
    DX12_ENUM_MAP_BEGIN(QueueType, D3D12_COMMAND_LIST_TYPE)
        DX12_ENUM_MAP_ITEM(QueueType::graphics, D3D12_COMMAND_LIST_TYPE_DIRECT)
        DX12_ENUM_MAP_ITEM(QueueType::compute, D3D12_COMMAND_LIST_TYPE_COMPUTE)
        DX12_ENUM_MAP_ITEM(QueueType::transfer, D3D12_COMMAND_LIST_TYPE_COPY)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(PixelFormat, DXGI_FORMAT)
        // 8-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::r8Unorm, DXGI_FORMAT_R8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::r8Snorm, DXGI_FORMAT_R8_SNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::r8Uint, DXGI_FORMAT_R8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::r8Sint, DXGI_FORMAT_R8_SINT)
        // 16-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::r16Uint, DXGI_FORMAT_R16_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::r16Sint, DXGI_FORMAT_R16_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::r16Float, DXGI_FORMAT_R16_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg8Unorm, DXGI_FORMAT_R8G8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg8Snorm, DXGI_FORMAT_R8G8_SNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg8Uint, DXGI_FORMAT_R8G8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg8Sint, DXGI_FORMAT_R8G8_SINT)
        // 32-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::r32Uint, DXGI_FORMAT_R32_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::r32Sint, DXGI_FORMAT_R32_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::r32Float, DXGI_FORMAT_R32_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg16Uint, DXGI_FORMAT_R16G16_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg16Sint, DXGI_FORMAT_R16G16_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg16Float, DXGI_FORMAT_R16G16_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba8Unorm, DXGI_FORMAT_R8G8B8A8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba8UnormSrgb, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba8Snorm, DXGI_FORMAT_R8G8B8A8_SNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba8Uint, DXGI_FORMAT_R8G8B8A8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba8Sint, DXGI_FORMAT_R8G8B8A8_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::bgra8Unorm, DXGI_FORMAT_B8G8R8A8_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::bgra8UnormSrgb, DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgb9E5Float, DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgb10A2Unorm, DXGI_FORMAT_R10G10B10A2_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg11B10Float, DXGI_FORMAT_R11G11B10_FLOAT)
        // 64-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::rg32Uint, DXGI_FORMAT_R32G32_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg32Sint, DXGI_FORMAT_R32G32_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rg32Float, DXGI_FORMAT_R32G32_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba16Uint, DXGI_FORMAT_R16G16B16A16_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba16Sint, DXGI_FORMAT_R16G16B16A16_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba16Float, DXGI_FORMAT_R16G16B16A16_FLOAT)
        // 128-Bits
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba32Uint, DXGI_FORMAT_R32G32B32A32_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba32Sint, DXGI_FORMAT_R32G32B32A32_SINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::rgba32Float, DXGI_FORMAT_R32G32B32A32_FLOAT)
        // Depth-Stencil
        DX12_ENUM_MAP_ITEM(PixelFormat::d16Unorm, DXGI_FORMAT_D16_UNORM)
        DX12_ENUM_MAP_ITEM(PixelFormat::d24UnormS8Uint, DXGI_FORMAT_D24_UNORM_S8_UINT)
        DX12_ENUM_MAP_ITEM(PixelFormat::d32Float, DXGI_FORMAT_D32_FLOAT)
        DX12_ENUM_MAP_ITEM(PixelFormat::d32FloatS8Uint, DXGI_FORMAT_D32_FLOAT_S8X24_UINT)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(VertexFormat, DXGI_FORMAT)
        // 8-Bits Channel
        DX12_ENUM_MAP_ITEM(VertexFormat::uint8X2, DXGI_FORMAT_R8G8_UINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::uint8X4, DXGI_FORMAT_R8G8B8A8_UINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint8X2, DXGI_FORMAT_R8G8_SINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint8X4, DXGI_FORMAT_R8G8B8A8_SINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::unorm8X2, DXGI_FORMAT_R8G8_UNORM)
        DX12_ENUM_MAP_ITEM(VertexFormat::unorm8X4, DXGI_FORMAT_R8G8B8A8_UNORM)
        DX12_ENUM_MAP_ITEM(VertexFormat::snorm8X2, DXGI_FORMAT_R8G8_SNORM)
        DX12_ENUM_MAP_ITEM(VertexFormat::snorm8X4, DXGI_FORMAT_R8G8B8A8_SNORM)
        // 16-Bits Channel
        DX12_ENUM_MAP_ITEM(VertexFormat::uint16X2, DXGI_FORMAT_R16G16_UINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::uint16X4, DXGI_FORMAT_R16G16B16A16_UINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint16X2, DXGI_FORMAT_R16G16_SINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint16X4, DXGI_FORMAT_R16G16B16A16_SINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::unorm16X2, DXGI_FORMAT_R16G16_UNORM)
        DX12_ENUM_MAP_ITEM(VertexFormat::unorm16X4, DXGI_FORMAT_R16G16B16A16_UNORM)
        DX12_ENUM_MAP_ITEM(VertexFormat::snorm16X2, DXGI_FORMAT_R16G16_SNORM)
        DX12_ENUM_MAP_ITEM(VertexFormat::snorm16X4, DXGI_FORMAT_R16G16B16A16_SNORM)
        DX12_ENUM_MAP_ITEM(VertexFormat::float16X2, DXGI_FORMAT_R16G16_FLOAT)
        DX12_ENUM_MAP_ITEM(VertexFormat::float16X4, DXGI_FORMAT_R16G16B16A16_FLOAT)
        // 32-Bits Channel
        DX12_ENUM_MAP_ITEM(VertexFormat::float32X1, DXGI_FORMAT_R32_FLOAT)
        DX12_ENUM_MAP_ITEM(VertexFormat::float32X2, DXGI_FORMAT_R32G32_FLOAT)
        DX12_ENUM_MAP_ITEM(VertexFormat::float32X3, DXGI_FORMAT_R32G32B32_FLOAT)
        DX12_ENUM_MAP_ITEM(VertexFormat::float32X4, DXGI_FORMAT_R32G32B32A32_FLOAT)
        DX12_ENUM_MAP_ITEM(VertexFormat::uint32X1, DXGI_FORMAT_R32_UINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::uint32X2, DXGI_FORMAT_R32G32_UINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::uint32X3, DXGI_FORMAT_R32G32B32_FLOAT)
        DX12_ENUM_MAP_ITEM(VertexFormat::uint32X4, DXGI_FORMAT_R32G32B32A32_UINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint32X1, DXGI_FORMAT_R32_SINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint32X2, DXGI_FORMAT_R32G32_SINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint32X3, DXGI_FORMAT_R32G32B32_SINT)
        DX12_ENUM_MAP_ITEM(VertexFormat::sint32X4, DXGI_FORMAT_R32G32B32A32_SINT)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(TextureViewDimension, D3D12_SRV_DIMENSION)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv1D, D3D12_SRV_DIMENSION_TEXTURE1D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv2D, D3D12_SRV_DIMENSION_TEXTURE2D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv2DArray, D3D12_SRV_DIMENSION_TEXTURE2DARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tvCube, D3D12_SRV_DIMENSION_TEXTURECUBE)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tvCubeArray, D3D12_SRV_DIMENSION_TEXTURECUBEARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv3D, D3D12_SRV_DIMENSION_TEXTURE3D)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(TextureViewDimension, D3D12_UAV_DIMENSION)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv1D, D3D12_UAV_DIMENSION_TEXTURE1D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv2D, D3D12_UAV_DIMENSION_TEXTURE2D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv2DArray, D3D12_UAV_DIMENSION_TEXTURE2DARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv3D, D3D12_UAV_DIMENSION_TEXTURE3D)
        // not support cube and cube array when texture is used as a UAV
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(TextureViewDimension, D3D12_RTV_DIMENSION)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv1D, D3D12_RTV_DIMENSION_TEXTURE1D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv2D, D3D12_RTV_DIMENSION_TEXTURE2D)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv2DArray, D3D12_RTV_DIMENSION_TEXTURE2DARRAY)
        DX12_ENUM_MAP_ITEM(TextureViewDimension::tv3D, D3D12_RTV_DIMENSION_TEXTURE3D)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(AddressMode, D3D12_TEXTURE_ADDRESS_MODE)
        DX12_ENUM_MAP_ITEM(AddressMode::clampToEdge, D3D12_TEXTURE_ADDRESS_MODE_CLAMP)
        DX12_ENUM_MAP_ITEM(AddressMode::repeat, D3D12_TEXTURE_ADDRESS_MODE_WRAP)
        DX12_ENUM_MAP_ITEM(AddressMode::mirrorRepeat, D3D12_TEXTURE_ADDRESS_MODE_MIRROR)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(ComparisonFunc, D3D12_COMPARISON_FUNC)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::never, D3D12_COMPARISON_FUNC_NEVER)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::less, D3D12_COMPARISON_FUNC_LESS)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::equal, D3D12_COMPARISON_FUNC_EQUAL)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::lessEqual, D3D12_COMPARISON_FUNC_LESS_EQUAL)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::greater, D3D12_COMPARISON_FUNC_GREATER)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::notEqual, D3D12_COMPARISON_FUNC_NOT_EQUAL)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::greaterEqual, D3D12_COMPARISON_FUNC_GREATER_EQUAL)
        DX12_ENUM_MAP_ITEM(ComparisonFunc::always, D3D12_COMPARISON_FUNC_ALWAYS)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(HlslBindingRangeType, D3D12_DESCRIPTOR_RANGE_TYPE)
        DX12_ENUM_MAP_ITEM(HlslBindingRangeType::constantBuffer, D3D12_DESCRIPTOR_RANGE_TYPE_CBV)
        DX12_ENUM_MAP_ITEM(HlslBindingRangeType::texture, D3D12_DESCRIPTOR_RANGE_TYPE_SRV)
        DX12_ENUM_MAP_ITEM(HlslBindingRangeType::sampler, D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER)
        DX12_ENUM_MAP_ITEM(HlslBindingRangeType::unorderedAccess, D3D12_DESCRIPTOR_RANGE_TYPE_UAV)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(ShaderStageBits, D3D12_SHADER_VISIBILITY)
        DX12_ENUM_MAP_ITEM(ShaderStageBits::sVertex, D3D12_SHADER_VISIBILITY_VERTEX)
        DX12_ENUM_MAP_ITEM(ShaderStageBits::sPixel, D3D12_SHADER_VISIBILITY_PIXEL)
        DX12_ENUM_MAP_ITEM(ShaderStageBits::sCompute, D3D12_SHADER_VISIBILITY_ALL)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(PrimitiveTopologyType, D3D12_PRIMITIVE_TOPOLOGY_TYPE)
        DX12_ENUM_MAP_ITEM(PrimitiveTopologyType::point, D3D12_PRIMITIVE_TOPOLOGY_TYPE_POINT)
        DX12_ENUM_MAP_ITEM(PrimitiveTopologyType::line, D3D12_PRIMITIVE_TOPOLOGY_TYPE_LINE)
        DX12_ENUM_MAP_ITEM(PrimitiveTopologyType::triangle, D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(PrimitiveTopology, D3D_PRIMITIVE_TOPOLOGY)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::pointList, D3D_PRIMITIVE_TOPOLOGY_POINTLIST)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::lineList, D3D_PRIMITIVE_TOPOLOGY_LINELIST)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::lineStrip, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::triangleList, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::triangleStrip, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::lineListAdj, D3D_PRIMITIVE_TOPOLOGY_LINELIST_ADJ)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::lineStripAdj, D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::triangleListAdj, D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST_ADJ)
        DX12_ENUM_MAP_ITEM(PrimitiveTopology::triangleStripAdj, D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP_ADJ)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(BlendOp, D3D12_BLEND_OP)
        DX12_ENUM_MAP_ITEM(BlendOp::opAdd, D3D12_BLEND_OP_ADD)
        DX12_ENUM_MAP_ITEM(BlendOp::opSubstract, D3D12_BLEND_OP_SUBTRACT)
        DX12_ENUM_MAP_ITEM(BlendOp::opReverseSubstract, D3D12_BLEND_OP_REV_SUBTRACT)
        DX12_ENUM_MAP_ITEM(BlendOp::opMin, D3D12_BLEND_OP_MIN)
        DX12_ENUM_MAP_ITEM(BlendOp::opMax, D3D12_BLEND_OP_MAX)
    DX12_ENUM_MAP_END()

    // TODO check this ?
    DX12_ENUM_MAP_BEGIN(BlendFactor, D3D12_BLEND)
        DX12_ENUM_MAP_ITEM(BlendFactor::zero, D3D12_BLEND_ZERO)
        DX12_ENUM_MAP_ITEM(BlendFactor::one, D3D12_BLEND_ONE)
        DX12_ENUM_MAP_ITEM(BlendFactor::src, D3D12_BLEND_SRC_COLOR)
        DX12_ENUM_MAP_ITEM(BlendFactor::oneMinusSrc, D3D12_BLEND_INV_SRC_COLOR)
        DX12_ENUM_MAP_ITEM(BlendFactor::srcAlpha, D3D12_BLEND_SRC_ALPHA)
        DX12_ENUM_MAP_ITEM(BlendFactor::oneMinusSrcAlpha, D3D12_BLEND_INV_SRC_ALPHA)
        DX12_ENUM_MAP_ITEM(BlendFactor::dst, D3D12_BLEND_DEST_ALPHA)
        DX12_ENUM_MAP_ITEM(BlendFactor::oneMinusDst, D3D12_BLEND_INV_DEST_COLOR)
        DX12_ENUM_MAP_ITEM(BlendFactor::dstAlpha, D3D12_BLEND_DEST_ALPHA)
        DX12_ENUM_MAP_ITEM(BlendFactor::oneMinusDstAlpha, D3D12_BLEND_INV_DEST_ALPHA)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(CullMode, D3D12_CULL_MODE)
        DX12_ENUM_MAP_ITEM(CullMode::none, D3D12_CULL_MODE_NONE)
        DX12_ENUM_MAP_ITEM(CullMode::front, D3D12_CULL_MODE_FRONT)
        DX12_ENUM_MAP_ITEM(CullMode::back, D3D12_CULL_MODE_BACK)
    DX12_ENUM_MAP_END()

    // see https://docs.microsoft.com/en-us/windows/win32/api/d3d12/ne-d3d12-d3d12_stencil_op
    DX12_ENUM_MAP_BEGIN(StencilOp, D3D12_STENCIL_OP)
        DX12_ENUM_MAP_ITEM(StencilOp::keep, D3D12_STENCIL_OP_KEEP)
        DX12_ENUM_MAP_ITEM(StencilOp::zero, D3D12_STENCIL_OP_ZERO)
        DX12_ENUM_MAP_ITEM(StencilOp::replace, D3D12_STENCIL_OP_REPLACE)
        DX12_ENUM_MAP_ITEM(StencilOp::invert, D3D12_STENCIL_OP_INVERT)
        DX12_ENUM_MAP_ITEM(StencilOp::incrementClamp, D3D12_STENCIL_OP_INCR_SAT)
        DX12_ENUM_MAP_ITEM(StencilOp::decrementClamp, D3D12_STENCIL_OP_DECR_SAT)
        DX12_ENUM_MAP_ITEM(StencilOp::incrementWrap, D3D12_STENCIL_OP_INCR)
        DX12_ENUM_MAP_ITEM(StencilOp::decrementWrap, D3D12_STENCIL_OP_DECR)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(IndexFormat, DXGI_FORMAT)
        DX12_ENUM_MAP_ITEM(IndexFormat::uint16, DXGI_FORMAT_R16_UINT)
        DX12_ENUM_MAP_ITEM(IndexFormat::uint32, DXGI_FORMAT_R32_UINT)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(PresentMode, DXGI_SWAP_EFFECT)
        DX12_ENUM_MAP_ITEM(PresentMode::immediately, DXGI_SWAP_EFFECT_FLIP_DISCARD)
        DX12_ENUM_MAP_ITEM(PresentMode::immediately, DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(VertexStepMode, D3D12_INPUT_CLASSIFICATION)
        DX12_ENUM_MAP_ITEM(VertexStepMode::perVertex, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA)
        DX12_ENUM_MAP_ITEM(VertexStepMode::perInstance, D3D12_INPUT_CLASSIFICATION_PER_INSTANCE_DATA)
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(BufferState, D3D12_RESOURCE_STATES)
        // TODO
    DX12_ENUM_MAP_END()

    DX12_ENUM_MAP_BEGIN(TextureState, D3D12_RESOURCE_STATES)
        DX12_ENUM_MAP_ITEM(TextureState::renderTarget, D3D12_RESOURCE_STATE_RENDER_TARGET)
        DX12_ENUM_MAP_ITEM(TextureState::present, D3D12_RESOURCE_STATE_PRESENT)
        DX12_ENUM_MAP_ITEM(TextureState::undefined, D3D12_RESOURCE_STATE_COMMON) // TODO It seems this cast is not corrrect
        DX12_ENUM_MAP_ITEM(TextureState::copyDst, D3D12_RESOURCE_STATE_COPY_DEST)
        DX12_ENUM_MAP_ITEM(TextureState::shaderReadOnly, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE)
    DX12_ENUM_MAP_END()

    // constant buffer size must be a multiple of 256
    inline size_t GetConstantBufferSize(size_t size) {
        return (size + (D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1)) & ~(D3D12_CONSTANT_BUFFER_DATA_PLACEMENT_ALIGNMENT - 1);
    }

    inline size_t GetBytesPerPixel(PixelFormat format) {
        if (format >= PixelFormat::r8Unorm && format <= PixelFormat::r8Sint) {
            return 1;
        }
        if (format >= PixelFormat::r16Uint && format <= PixelFormat::rg8Sint) {
            return 2;
        }
        if (format >= PixelFormat::r32Uint && format <= PixelFormat::rg11B10Float) {
            return 4;
        }
        if (format >= PixelFormat::rg32Uint && format <= PixelFormat::rgba16Float) {
            return 8;
        }
        if (format >= PixelFormat::rgba32Uint && format <= PixelFormat::rgba32Float) {
            return 16;
        }

        // TODO depth stencil texture format

        return 0;
    }
}
