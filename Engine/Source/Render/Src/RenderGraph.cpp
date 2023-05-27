//
// Created by johnk on 2023/2/10.
//

#include <utility>
#include <unordered_set>
#include <optional>

#include <Render/RenderGraph.h>

namespace Render {
    static std::vector<RHI::GraphicsPassColorAttachment> GetRHIColorAttachments(const RGRasterPassDesc& desc)
    {
        std::vector<RHI::GraphicsPassColorAttachment> result(desc.colorAttachments.size());
        for (auto i = 0; i < result.size(); i++) {
            auto& dst = result[i];
            const auto& src = desc.colorAttachments[i];

            dst.view = src.view == nullptr ? nullptr : src.view->GetRHI();
            dst.resolve = src.resolve == nullptr ? nullptr : src.resolve->GetRHI();
            dst.clearValue = src.clearValue;
            dst.loadOp = src.loadOp;
            dst.storeOp = src.storeOp;
        }
        return result;
    }

    static std::optional<RHI::GraphicsPassDepthStencilAttachment> GetRHIDepthStencilAttachment(const RGRasterPassDesc& desc)
    {
        if (!desc.depthStencilAttachment.has_value()) {
            return {};
        }

        const auto& src = desc.depthStencilAttachment.value();

        RHI::GraphicsPassDepthStencilAttachment result;
        result.view = src.view == nullptr ? nullptr : src.view->GetRHI();
        result.depthClearValue = src.depthClearValue;
        result.depthLoadOp = src.depthLoadOp;
        result.depthStoreOp = src.depthStoreOp;
        result.depthReadOnly = src.depthReadOnly;
        result.stencilClearValue = src.stencilClearValue;
        result.stencilLoadOp = src.stencilLoadOp;
        result.stencilStoreOp = src.stencilStoreOp;
        result.stencilReadOnly = src.stencilReadOnly;
        return result;
    }

    static RHI::GraphicsPassBeginInfo GetRHIGraphicsPassBeginInfo(
        const RGRasterPassDesc& desc,
        uint32_t colorAttachmentNum,
        const RHI::GraphicsPassColorAttachment* colorAttachments,
        const RHI::GraphicsPassDepthStencilAttachment* depthStencilAttachment)
    {
        RHI::GraphicsPassBeginInfo result;
        result.colorAttachmentNum = colorAttachmentNum;
        result.colorAttachments = colorAttachments;
        result.depthStencilAttachment = depthStencilAttachment;
        return result;
    }

    static RHI::Barrier GetBarrier(const RGResTransition& transition)
    {
        if (transition.resType == RHI::ResourceType::buffer) {
            const auto& bufferTransition = transition.buffer;
            return RHI::Barrier::Transition(bufferTransition.buffer->GetRHI(), bufferTransition.before, bufferTransition.after);
        }
        if (transition.resType == RHI::ResourceType::texture) {
            const auto& textureTransition = transition.texture;
            return RHI::Barrier::Transition(textureTransition.texture->GetRHI(), textureTransition.before, textureTransition.after);
        }
        Assert(false);
        return {};
    }

    static RHI::BufferCreateInfo GetRHIBufferCreateInfo(const RGBufferDesc& desc)
    {
        RHI::BufferCreateInfo createInfo;
        createInfo.size = desc.size;
        createInfo.usages = desc.usages;
        return createInfo;
    }

    static RHI::TextureCreateInfo GetRHITextureCreateInfo(const RGTextureDesc& desc)
    {
        RHI::TextureCreateInfo createInfo;
        createInfo.extent = desc.extent;
        createInfo.mipLevels = desc.mipLevels;
        createInfo.samples = desc.samples;
        createInfo.dimension = desc.dimension;
        createInfo.format = desc.format;
        createInfo.usages = desc.usages;
        return createInfo;
    }

    static RHI::BufferViewCreateInfo GetRHIBufferViewCreateInfo(const RGBufferViewDesc& desc)
    {
        RHI::BufferViewCreateInfo createInfo;
        createInfo.offset = desc.offset;
        createInfo.size = desc.size;
        createInfo.vertex.stride = desc.vertex.stride;
        return createInfo;
    }

    static RHI::TextureViewCreateInfo GetRHITextureViewCreateInfo(const RGTextureViewDesc& desc)
    {
        RHI::TextureViewCreateInfo createInfo;
        createInfo.dimension = desc.dimension;
        createInfo.aspect = desc.aspect;
        createInfo.baseMipLevel = desc.baseMipLevel;
        createInfo.mipLevelNum = desc.mipLevelNum;
        createInfo.baseArrayLayer = desc.baseArrayLayer;
        createInfo.arrayLayerNum = desc.arrayLayerNum;
        return createInfo;
    }
}

namespace Render {
    RGResTransition RGResTransition::Buffer(RGBuffer* inBuffer, RGBufferState inBeforeState, RGBufferState inAfterState)
    {
        RGResTransition result;
        result.resType = RGTransitionResType::buffer;
        result.buffer.buffer = inBuffer;
        result.buffer.before = inBeforeState;
        result.buffer.after = inAfterState;
        return result;
    }

    RGResTransition RGResTransition::Texture(RGTexture* inTexture, RGTextureState inBeforeState, RGTextureState inAfterState)
    {
        RGResTransition result;
        result.resType = RGTransitionResType::texture;
        result.texture.texture = inTexture;
        result.texture.before = inBeforeState;
        result.texture.after = inAfterState;
        return result;
    }

    RGResource::RGResource(std::string inName, bool inIsExternal, RGResource* inParent)
        : name(std::move(inName))
        , parent(inParent)
        , isExternal(inIsExternal)
        , rhiAccess(false)
    {
    }

    RGBufferDesc RGBufferDesc::Create(size_t size, RHI::BufferUsageFlags usages)
    {
        RGBufferDesc result;
        result.size = size;
        result.usages = usages;
        return result;
    }

    RGTextureDesc RGTextureDesc::Create1D(uint32_t length, RHI::PixelFormat format, RHI::TextureUsageFlags usages, uint8_t mipLevels, uint8_t samples)
    {
        RGTextureDesc result;
        result.extent = { length, 1, 1 };
        result.mipLevels = mipLevels;
        result.samples = samples;
        result.dimension = RHI::TextureDimension::t1D;
        result.format = format;
        result.usages = usages;
        return result;
    }

    RGTextureDesc RGTextureDesc::Create2D(uint32_t width, uint32_t height, RHI::PixelFormat format, RHI::TextureUsageFlags usages, uint32_t layers, uint8_t mipLevels, uint8_t samples)
    {
        RGTextureDesc result;
        result.extent = { width, height, layers };
        result.mipLevels = mipLevels;
        result.samples = samples;
        result.dimension = RHI::TextureDimension::t2D;
        result.format = format;
        result.usages = usages;
        return result;
    }

    RGTextureDesc RGTextureDesc::Create3D(uint32_t width, uint32_t height, uint32_t depth, RHI::PixelFormat format, RHI::TextureUsageFlags usages, uint8_t mipLevels, uint8_t samples)
    {
        RGTextureDesc result;
        result.extent = { width, height, depth };
        result.mipLevels = mipLevels;
        result.samples = samples;
        result.dimension = RHI::TextureDimension::t3D;
        result.format = format;
        result.usages = usages;
        return result;
    }

    RGBufferViewDesc RGBufferViewDesc::Create(size_t size, size_t offset)
    {
        RGBufferViewDesc result;
        result.offset = offset;
        result.size = size;
        return result;
    }

    RGBufferViewDesc RGBufferViewDesc::CreateVertex(size_t stride, size_t size, size_t offset)
    {
        RGBufferViewDesc result;
        result.offset = offset;
        result.size = size;
        result.vertex.stride = stride;
        return result;
    }

    RGBufferViewDesc RGBufferViewDesc::CreateIndex(RHI::IndexFormat indexFormat, size_t size, size_t offset)
    {
        RGBufferViewDesc result;
        result.offset = offset;
        result.size = size;
        result.index.format = indexFormat;
        return result;
    }

    std::pair<RGBuffer*, RGBufferViewDesc> RGBufferViewDesc::Create(RGBuffer* buffer, size_t size, size_t offset)
    {
        RGBufferViewDesc result;
        result.offset = offset;
        result.size = size == UINT64_MAX ? buffer->GetDesc().size : size;
        return std::make_pair(buffer, result);
    }

    std::pair<RGBuffer*, RGBufferViewDesc> RGBufferViewDesc::CreateVertex(RGBuffer* buffer, size_t stride, size_t size, size_t offset)
    {
        RGBufferViewDesc result;
        result.offset = offset;
        result.size = size == UINT64_MAX ? buffer->GetDesc().size : size;
        result.vertex.stride = stride;
        return std::make_pair(buffer, result);
    }

    std::pair<RGBuffer*, RGBufferViewDesc> RGBufferViewDesc::CreateIndex(RGBuffer* buffer, RHI::IndexFormat indexFormat, size_t size, size_t offset)
    {
        RGBufferViewDesc result;
        result.offset = offset;
        result.size = size == UINT64_MAX ? buffer->GetDesc().size : size;
        result.index.format = indexFormat;
        return std::make_pair(buffer, result);
    }

    RGTextureViewDesc RGTextureViewDesc::Create1D(RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv1D;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 1;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::Create2D(RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv2D;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 1;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::Create2DArray(RHI::TextureAspect aspect, uint8_t baseArrayLayer, uint8_t arrayLayerNum, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv2DArray;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum;
        result.baseArrayLayer = baseArrayLayer;
        result.arrayLayerNum = arrayLayerNum;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::CreateCube(RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tvCube;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 6;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::CreateCubeArray(RHI::TextureAspect aspect, uint8_t baseCubemapIndex, uint8_t cubemapNum, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tvCubeArray;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum;
        result.baseArrayLayer = baseCubemapIndex * 6;
        result.arrayLayerNum = cubemapNum * 6;
        return result;
    }

    RGTextureViewDesc RGTextureViewDesc::Create3D(RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv3D;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 1;
        return result;
    }

    std::pair<RGTexture*, RGTextureViewDesc> RGTextureViewDesc::Create1D(RGTexture* texture, RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv1D;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum == UINT8_MAX ? texture->GetDesc().mipLevels : mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 1;
        return std::make_pair(texture, result);
    }

    std::pair<RGTexture*, RGTextureViewDesc> RGTextureViewDesc::Create2D(RGTexture* texture, RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv2D;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum == UINT8_MAX ? texture->GetDesc().mipLevels : mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 1;
        return std::make_pair(texture, result);
    }

    std::pair<RGTexture*, RGTextureViewDesc> RGTextureViewDesc::Create2DArray(RGTexture* texture, RHI::TextureAspect aspect, uint8_t baseArrayLayer, uint8_t arrayLayerNum, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv2DArray;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum == UINT8_MAX ? texture->GetDesc().mipLevels : mipLevelNum;
        result.baseArrayLayer = baseArrayLayer;
        result.arrayLayerNum = arrayLayerNum == UINT8_MAX ? texture->GetDesc().extent.z : arrayLayerNum;
        return std::make_pair(texture, result);
    }

    std::pair<RGTexture*, RGTextureViewDesc> RGTextureViewDesc::CreateCube(RGTexture* texture, RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tvCube;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum == UINT8_MAX ? texture->GetDesc().mipLevels : mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 6;
        return std::make_pair(texture, result);
    }

    std::pair<RGTexture*, RGTextureViewDesc> RGTextureViewDesc::CreateCubeArray(RGTexture* texture, RHI::TextureAspect aspect, uint8_t baseCubemapIndex, uint8_t cubemapNum, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tvCubeArray;
        result.aspect = aspect;
        result.baseMipLevel = baseMipLevel;
        result.mipLevelNum = mipLevelNum == UINT8_MAX ? texture->GetDesc().mipLevels : mipLevelNum;
        result.baseArrayLayer = baseCubemapIndex * 6;
        result.arrayLayerNum = (cubemapNum == UINT8_MAX ? (texture->GetDesc().extent.z / 6) : cubemapNum) * 6;
        return std::make_pair(texture, result);
    }

    std::pair<RGTexture*, RGTextureViewDesc> RGTextureViewDesc::Create3D(RGTexture* texture, RHI::TextureAspect aspect, uint8_t baseMipLevel, uint8_t mipLevelNum)
    {
        RGTextureViewDesc result;
        result.dimension = RHI::TextureViewDimension::tv3D;
        result.aspect = aspect;
        result.baseMipLevel = mipLevelNum == UINT8_MAX ? texture->GetDesc().mipLevels : mipLevelNum;
        result.baseArrayLayer = 0;
        result.arrayLayerNum = 1;
        return std::make_pair(texture, result);
    }

    RGBindItem RGBindItem::Sampler(RGSampler* sampler)
    {
        RGBindItem result;
        result.type = RHI::BindingType::sampler;
        result.sampler = sampler;
        return result;
    }

    RGBindItem RGBindItem::Texture(RGTextureView* textureView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::texture;
        result.textureView = textureView;
        return result;
    }

    RGBindItem RGBindItem::StorageTexture(RGTextureView* textureView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::storagetTexture;
        result.textureView = textureView;
        return result;
    }

    RGBindItem RGBindItem::UniformBuffer(RGBufferView* bufferView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::uniformBuffer;
        result.bufferView = bufferView;
        return result;
    }

    RGBindItem RGBindItem::StorageBuffer(RGBufferView* bufferView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::uniformBuffer;
        result.bufferView = bufferView;
        return result;
    }

    std::pair<std::string, RGBindItem> RGBindItem::Sampler(std::string name, RGSampler* sampler)
    {
        RGBindItem result;
        result.type = RHI::BindingType::sampler;
        result.sampler = sampler;
        return std::make_pair(std::move(name), result);
    }

    std::pair<std::string, RGBindItem> RGBindItem::Texture(std::string name, RGTextureView* textureView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::texture;
        result.textureView = textureView;
        return std::make_pair(std::move(name), result);
    }

    std::pair<std::string, RGBindItem> RGBindItem::StorageTexture(std::string name, RGTextureView* textureView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::storagetTexture;
        result.textureView = textureView;
        return std::make_pair(std::move(name), result);
    }

    std::pair<std::string, RGBindItem> RGBindItem::UniformBuffer(std::string name, RGBufferView* bufferView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::uniformBuffer;
        result.bufferView = bufferView;
        return std::make_pair(std::move(name), result);
    }

    std::pair<std::string, RGBindItem> RGBindItem::StorageBuffer(std::string name, RGBufferView* bufferView)
    {
        RGBindItem result;
        result.type = RHI::BindingType::storageBuffer;
        result.bufferView = bufferView;
        return std::make_pair(std::move(name), result);
    }

    RGResource::~RGResource() = default;

    bool RGResource::IsExternal() const
    {
        return isExternal;
    }

    bool RGResource::IsCulled() const
    {
        return isCulled;
    }

    const std::string& RGResource::GetName() const
    {
        return name;
    }

    bool RGResource::CanAccessRHI() const
    {
        return rhiAccess;
    }

    RGResource* RGResource::GetParent() const
    {
        return parent;
    }

    void RGResource::SetCulled(bool inCulled)
    {
        isCulled = inCulled;
    }

    void RGResource::SetRHIAccess(bool inRhiAccess)
    {
        rhiAccess = inRhiAccess;
    }

    RGBuffer::RGBuffer(std::string inName, const RGBufferDesc& inDesc)
        : RGResource(std::move(inName), false)
        , desc(inDesc)
    {
    }

    RGBuffer::RGBuffer(std::string inName, RHI::Buffer* inBuffer)
        : RGResource(std::move(inName), true)
        , rhiHandle(inBuffer)
    {
    }

    RGBuffer::~RGBuffer() = default;

    RGResourceType RGBuffer::GetType()
    {
        return RGResourceType::buffer;
    }

    void RGBuffer::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHIBufferCreateInfo(desc);
            rhiHandle = device.CreateBuffer(createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGBuffer::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::Buffer* RGBuffer::GetRHI() const
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    const RGBufferDesc& RGBuffer::GetDesc() const
    {
        return desc;
    }

    RGTexture::RGTexture(std::string inName, const RGTextureDesc& inDesc)
        : RGResource(std::move(inName), false)
        , desc(inDesc)
    {
    }

    RGTexture::RGTexture(std::string inName, RHI::Texture* inTexture)
        : RGResource(std::move(inName), true)
        , rhiHandle(inTexture)
    {
    }

    RGTexture::~RGTexture() = default;

    RGResourceType RGTexture::GetType()
    {
        return RGResourceType::texture;
    }

    void RGTexture::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHITextureCreateInfo(desc);
            rhiHandle = device.CreateTexture(createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGTexture::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::Texture* RGTexture::GetRHI() const
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    const RGTextureDesc& RGTexture::GetDesc() const
    {
        return desc;
    }

    RGBufferView::RGBufferView(RGBuffer* inBuffer, const RGBufferViewDesc& inDesc)
        : RGResource(inBuffer->GetName() + "View", false, inBuffer)
        , buffer(inBuffer)
        , desc(inDesc)
    {
    }

    RGBufferView::RGBufferView(const std::pair<RGBuffer*, RGBufferViewDesc>& bufferAndViewDesc)
        : RGResource(bufferAndViewDesc.first->GetName() + "View", false, bufferAndViewDesc.first)
        , buffer(bufferAndViewDesc.first)
        , desc(bufferAndViewDesc.second)
    {
    }

    RGBufferView::RGBufferView(std::string inName, RGBuffer* inBuffer, const RGBufferViewDesc& inDesc)
        : RGResource(std::move(inName), false, inBuffer)
        , buffer(inBuffer)
        , desc(inDesc)
    {
    }

    RGBufferView::RGBufferView(std::string inName, const std::pair<RGBuffer*, RGBufferViewDesc>& bufferAndViewDesc)
        : RGResource(std::move(inName), false, bufferAndViewDesc.first)
        , buffer(bufferAndViewDesc.first)
        , desc(bufferAndViewDesc.second)
    {
    }

    RGBufferView::RGBufferView(std::string inName, RHI::BufferView* inBufferView)
        : RGResource(std::move(inName), true)
        , rhiHandle(inBufferView)
    {
    }

    RGBufferView::~RGBufferView() = default;

    RGResourceType RGBufferView::GetType()
    {
        return RGResourceType::bufferView;
    }

    void RGBufferView::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHIBufferViewCreateInfo(desc);
            Assert(buffer);
            Assert(buffer->GetRHI());
            rhiHandle = buffer->GetRHI()->CreateBufferView(createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGBufferView::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::BufferView* RGBufferView::GetRHI() const
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    RGBuffer* RGBufferView::GetBuffer() const
    {
        Assert(!IsExternal());
        return buffer;
    }

    const RGBufferViewDesc& RGBufferView::GetDesc() const
    {
        return desc;
    }

    RGTextureView::RGTextureView(RGTexture* inTexture, const RGTextureViewDesc& inDesc)
        : RGResource(inTexture->GetName() + "View", false, inTexture)
        , texture(inTexture)
        , desc(inDesc)
    {
    }

    RGTextureView::RGTextureView(const std::pair<RGTexture*, RGTextureViewDesc>& textureAndViewDesc)
        : RGResource(textureAndViewDesc.first->GetName() + "View", false, textureAndViewDesc.first)
        , texture(textureAndViewDesc.first)
        , desc(textureAndViewDesc.second)
    {
    }

    RGTextureView::RGTextureView(std::string inName, RGTexture* inTexture, const RGTextureViewDesc& inDesc)
        : RGResource(std::move(inName), false, inTexture)
        , texture(inTexture)
        , desc(inDesc)
    {
    }

    RGTextureView::RGTextureView(std::string inName, const std::pair<RGTexture*, RGTextureViewDesc>& textureAndViewDesc)
        : RGResource(std::move(inName), false, textureAndViewDesc.first)
        , texture(textureAndViewDesc.first)
        , desc(textureAndViewDesc.second)
    {
    }

    RGTextureView::RGTextureView(std::string inName, RHI::TextureView* inTextureView)
        : RGResource(std::move(inName), true)
        , rhiHandle(inTextureView)
    {
    }

    RGTextureView::~RGTextureView() = default;

    RGResourceType RGTextureView::GetType()
    {
        return RGResourceType::textureView;
    }

    void RGTextureView::Devirtualize(RHI::Device& device)
    {
        if (rhiHandle == nullptr && !IsExternal()) {
            auto createInfo = GetRHITextureViewCreateInfo(desc);
            Assert(texture);
            Assert(texture->GetRHI());
            rhiHandle = texture->GetRHI()->CreateTextureView(createInfo);
        }

        Assert(rhiHandle);
        SetRHIAccess(true);
    }

    void RGTextureView::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::TextureView* RGTextureView::GetRHI() const
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    RGTexture* RGTextureView::GetTexture() const
    {
        Assert(!IsExternal());
        return texture;
    }

    const RGTextureViewDesc& RGTextureView::GetDesc() const
    {
        return desc;
    }

    RGBindGroup::RGBindGroup(RGBindGroupDesc inDesc)
        : RGResource("", false, nullptr)
        , desc(std::move(inDesc))
    {
    }

    RGBindGroup::RGBindGroup(RHI::BindGroup* inBindGroup)
        : RGResource("", true, nullptr)
        , rhiHandle(inBindGroup)
    {
    }

    RGBindGroup::RGBindGroup(std::string inName, RGBindGroupDesc inDesc)
        : RGResource(std::move(inName), false, nullptr)
        , desc(std::move(inDesc))
    {
    }

    RGBindGroup::RGBindGroup(std::string inName, RHI::BindGroup* inBindGroup)
        : RGResource(std::move(inName), true, nullptr)
        , rhiHandle(inBindGroup)
    {
    }

    RGBindGroup::~RGBindGroup() = default;

    RGResourceType RGBindGroup::GetType()
    {
        return RGResourceType::bindGroup;
    }

    void RGBindGroup::Devirtualize(RHI::Device& device)
    {
        // TODO
    }

    void RGBindGroup::Destroy()
    {
        if (!IsExternal() && rhiHandle != nullptr) {
            rhiHandle->Destroy();
        }
    }

    RHI::BindGroup* RGBindGroup::GetRHI() const
    {
        Assert(CanAccessRHI());
        return rhiHandle;
    }

    const RGBindGroupDesc& RGBindGroup::GetDesc() const
    {
        return desc;
    }

    RGPass::RGPass(std::string inName)
        : name(std::move(inName))
    {
    }

    RGPass::~RGPass() = default;

    RGCopyPass::RGCopyPass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGCopyPass::~RGCopyPass() = default;

    RGPassType RGCopyPass::GetType()
    {
        return RGPassType::copy;
    }

    RGComputePass::RGComputePass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGComputePass::~RGComputePass() = default;

    RGPassType RGComputePass::GetType()
    {
        return RGPassType::compute;
    }

    RGRasterPass::RGRasterPass(std::string inName)
        : RGPass(std::move(inName))
    {
    }

    RGRasterPass::~RGRasterPass() = default;

    RGPassType RGRasterPass::GetType()
    {
        return RGPassType::raster;
    }

    RGFuncCopyPass::RGFuncCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc)
        : RGCopyPass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
    {
    }

    RGFuncCopyPass::~RGFuncCopyPass() = default;

    void RGFuncCopyPass::Setup(RGCopyPassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncCopyPass::Execute(RHI::CommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RGFuncComputePass::RGFuncComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc)
        : RGComputePass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
        , executeFunc(nullptr)
    {
    }

    RGFuncComputePass::~RGFuncComputePass() = default;

    void RGFuncComputePass::Setup(RGComputePassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncComputePass::Execute(RHI::ComputePassCommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RGFuncRasterPass::RGFuncRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc)
        : RGRasterPass(std::move(inName))
        , setupFunc(std::move(inSetupFunc))
    {
    }

    RGFuncRasterPass::~RGFuncRasterPass() = default;

    void RGFuncRasterPass::Setup(RGRasterPassBuilder& builder)
    {
        executeFunc = setupFunc(builder);
    }

    void RGFuncRasterPass::Execute(RHI::GraphicsPassCommandEncoder& encoder)
    {
        Assert(executeFunc != nullptr);
        executeFunc(encoder);
    }

    RenderGraph::RenderGraph(RHI::Device& inDevice)
        : device(inDevice)
    {
    }

    RenderGraph::~RenderGraph() = default;

    void RenderGraph::AddCopyPass(RGCopyPass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddComputePass(RGComputePass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddRasterPass(RGRasterPass* inPass)
    {
        passes.emplace_back(inPass);
    }

    void RenderGraph::AddCopyPass(std::string inName, RGCopyPassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncCopyPass(std::move(inName), std::move(inSetupFunc)));
    }

    void RenderGraph::AddComputePass(std::string inName, RGComputePassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncComputePass(std::move(inName), std::move(inSetupFunc)));
    }

    void RenderGraph::AddRasterPass(std::string inName, RGRasterPassSetupFunc inSetupFunc)
    {
        passes.emplace_back(new RGFuncRasterPass(std::move(inName), std::move(inSetupFunc)));
    }

    RHI::Device& RenderGraph::GetDevice()
    {
        return device;
    }

    void RenderGraph::Setup()
    {
        for (auto& pass : passes) {
            if (pass->GetType() == RGPassType::copy) {
                auto* copyPass = static_cast<RGCopyPass*>(pass.Get());
                RGCopyPassBuilder builder(*this, *copyPass);
                copyPass->Setup(builder);
            } else if (pass->GetType() == RGPassType::compute) {
                auto* computePass = static_cast<RGComputePass*>(pass.Get());
                RGComputePassBuilder builder(*this, *computePass);
                computePass->Setup(builder);
            } else if (pass->GetType() == RGPassType::raster) {
                auto* rasterPass = static_cast<RGRasterPass*>(pass.Get());
                RGRasterPassBuilder builder(*this, *rasterPass);
                rasterPass->Setup(builder);
            } else {
                Assert(false);
            }
        }
    }

    void RenderGraph::Compile()
    {
        for (auto& resource : resources) {
            resource->Destroy();
            resource->SetCulled(false);
        }

        std::unordered_set<RGResource*> consumeds;
        for (auto& pass : passes) {
            for (const auto& read : pass->reads) {
                consumeds.emplace(read);
            }
        }

        for (auto& resource : resources) {
            if (!consumeds.contains(resource.Get())) {
                resource->SetCulled(true);
            }
        }

        ComputeResBarriers();
    }

    void RenderGraph::Execute(RHI::Fence* mainFence, RHI::Fence* asyncComputeFence, RHI::Fence* asyncCopyFence) // NOLINT
    {
        std::vector<RGResource*> actualResesToDevirtualize;
        std::vector<RGResource*> actualResViewsToDevirtualize;

        for (auto& resource : resources) {
            resource->SetRHIAccess(false);

            auto type = resource->GetType();
            if (type == RGResourceType::buffer || type == RGResourceType::texture) {
                actualResesToDevirtualize.emplace_back(resource.Get());
            }
            if (type == RGResourceType::bufferView || type == RGResourceType::textureView) {
                actualResViewsToDevirtualize.emplace_back(resource.Get());
            }
        }

        for (auto* resource : actualResesToDevirtualize) {
            if (!resource->isCulled) {
                resource->Devirtualize(device);
            }
        }
        for (auto* resource : actualResViewsToDevirtualize) {
            if (!resource->isCulled) {
                resource->Devirtualize(device);
            }
        }

        Assert(device.GetQueueNum(RHI::QueueType::graphics) > 0);
        RHI::Queue* mainQueue = device.GetQueue(RHI::QueueType::graphics, 0);
        RHI::Queue* asyncComputeQueue = device.GetQueueNum(RHI::QueueType::compute) > 1 ? device.GetQueue(RHI::QueueType::compute, 1) : mainQueue;
        RHI::Queue* asyncCopyQueue = device.GetQueueNum(RHI::QueueType::graphics) > 1 ? device.GetQueue(RHI::QueueType::compute, 1) : mainQueue;

        Common::UniqueRef<RHI::CommandBuffer> mainBuffer = device.CreateCommandBuffer();
        Common::UniqueRef<RHI::CommandBuffer> asyncComputeBuffer = device.CreateCommandBuffer();
        Common::UniqueRef<RHI::CommandBuffer> asyncCopyBuffer = device.CreateCommandBuffer();
        Common::UniqueRef<RHI::CommandEncoder> mainEncoder = mainBuffer->Begin();
        Common::UniqueRef<RHI::CommandEncoder> asyncComputeEncoder = asyncComputeBuffer->Begin();
        Common::UniqueRef<RHI::CommandEncoder> asyncCopyEncoder = asyncCopyBuffer->Begin();
        {
            for (auto& pass : passes) {
                if (pass->GetType() == RGPassType::copy) {
                    auto* copyPass = static_cast<RGCopyPass*>(pass.Get());
                    auto& commandEncoder = copyPass->isAsyncCopy ? asyncCopyEncoder : mainEncoder;
                    TransitionResources(commandEncoder.Get(), copyPass);
                    ExecuteCopyPass(commandEncoder.Get(), copyPass);
                } else if (pass->GetType() == RGPassType::raster) {
                    auto* computePass = static_cast<RGComputePass*>(pass.Get());
                    auto& commandEncoder = computePass->isAsyncCompute ? asyncComputeEncoder : mainEncoder;
                    TransitionResources(commandEncoder.Get(), computePass);
                    ExecuteComputePass(commandEncoder.Get(), computePass);
                } else if (pass->GetType() == RGPassType::compute) {
                    auto* rasterPass = static_cast<RGRasterPass*>(pass.Get());
                    TransitionResources(mainEncoder.Get(), rasterPass);
                    ExecuteRasterPass(mainEncoder.Get(), rasterPass);
                } else {
                    Assert(false);
                }
            }
        }
        mainEncoder->End();
        asyncComputeEncoder->End();
        asyncCopyEncoder->End();

        mainQueue->Submit(mainBuffer.Get(), mainFence);
        asyncComputeQueue->Submit(asyncComputeBuffer.Get(), asyncComputeFence);
        asyncCopyQueue->Submit(asyncCopyBuffer.Get(), asyncCopyFence);
    }

    void RenderGraph::ExecuteCopyPass(RHI::CommandEncoder* encoder, RGCopyPass* copyPass)
    {
        copyPass->Execute(*encoder);
    }

    void RenderGraph::ExecuteComputePass(RHI::CommandEncoder* encoder, RGComputePass* computePass)
    {
        Common::UniqueRef<RHI::ComputePassCommandEncoder> computeEncoder = encoder->BeginComputePass();
        {
            computePass->Execute(*computeEncoder);
        }
        computeEncoder->EndPass();
    }

    void RenderGraph::ExecuteRasterPass(RHI::CommandEncoder* encoder, RGRasterPass* rasterPass)
    {
        std::vector<RHI::GraphicsPassColorAttachment> colorAttachments = GetRHIColorAttachments(rasterPass->passDesc);
        std::optional<RHI::GraphicsPassDepthStencilAttachment> depthStencilAttachment = GetRHIDepthStencilAttachment(rasterPass->passDesc);
        RHI::GraphicsPassBeginInfo beginInfo = GetRHIGraphicsPassBeginInfo(
            rasterPass->passDesc,
            colorAttachments.size(),
            colorAttachments.data(),
            depthStencilAttachment.has_value() ? &depthStencilAttachment.value() : nullptr);
        Common::UniqueRef<RHI::GraphicsPassCommandEncoder> graphicsEncoder = encoder->BeginGraphicsPass(&beginInfo);
        {
            rasterPass->Execute(*graphicsEncoder);
        }
        graphicsEncoder->EndPass();
    }

    RGBuffer* RenderGraph::GetActualBufferRes(RGResource* res)
    {
        if (res->GetType() == RGResourceType::buffer) {
            return static_cast<RGBuffer*>(res);
        }
        if (res->GetType() == RGResourceType::bufferView) {
            return static_cast<RGBufferView*>(res)->GetBuffer();
        }
        Assert(false);
        return nullptr;
    }

    RGTexture* RenderGraph::GetActualTextureRes(RGResource* res)
    {
        if (res->GetType() == RGResourceType::texture) {
            return static_cast<RGTexture*>(res);
        }
        if (res->GetType() == RGResourceType::textureView) {
            return static_cast<RGTextureView*>(res)->GetTexture();
        }
        Assert(false);
        return nullptr;
    }

    RHI::BufferState RenderGraph::ComputeBufferState(RGPassType passType, RGResourceAccessType accessType)
    {
        if (passType == RGPassType::copy) {
            if (accessType == RGResourceAccessType::read) {
                return RHI::BufferState::copySrc;
            }
            if (accessType == RGResourceAccessType::write) {
                return RHI::BufferState::copyDst;
            }
        } else {
            if (accessType == RGResourceAccessType::read) {
                return RHI::BufferState::shaderReadOnly;
            }
            if (accessType == RGResourceAccessType::write) {
                return RHI::BufferState::storage;
            }
        }
        return RHI::BufferState::undefined;
    }

    RHI::TextureState RenderGraph::ComputeTextureState(RGPassType passType, RGResourceAccessType accessType)
    {
        if (passType == RGPassType::copy) {
            if (accessType == RGResourceAccessType::read) {
                return RHI::TextureState::copySrc;
            }
            if (accessType == RGResourceAccessType::write) {
                return RHI::TextureState::copyDst;
            }
        }
        if (passType == RGPassType::compute) {
            if (accessType == RGResourceAccessType::read) {
                return RHI::TextureState::shaderReadOnly;
            }
            if (accessType == RGResourceAccessType::write) {
                return RHI::TextureState::storage;
            }
        }
        if (passType == RGPassType::raster) {
            if (accessType == RGResourceAccessType::read) {
                return RHI::TextureState::shaderReadOnly;
            }
            if (accessType == RGResourceAccessType::write) {
                return RHI::TextureState::renderTarget;
            }
        }
        return RHI::TextureState::undefined;
    }

    void RenderGraph::ComputeResBarriers()
    {
        LastResStates lastResStates;
        for (const auto& pass : passes) {
            ComputeResTransitionsByAccessGroup<RGResourceAccessType::read>(pass.Get(), pass->reads, lastResStates);
            ComputeResTransitionsByAccessGroup<RGResourceAccessType::write>(pass.Get(), pass->writes, lastResStates);
            UpdateLastResStatesByAccessGroup<RGResourceAccessType::read>(pass->GetType(), pass->reads, lastResStates);
            UpdateLastResStatesByAccessGroup<RGResourceAccessType::write>(pass->GetType(), pass->writes, lastResStates);
        }
    }

    void RenderGraph::TransitionResources(RHI::CommandEncoder* encoder, RGPass* pass)
    {
        // TODO skip vertex buffer and index buffer
        std::vector<RGResource*> transitionResources;
        for (auto* read : pass->reads) {
            transitionResources.emplace_back(read);
        }
        for (auto* write : pass->writes) {
            transitionResources.emplace_back(write);
        }
        for (auto* res : transitionResources) {
            encoder->ResourceBarrier(GetBarrier(resTransitionMap[std::make_pair(res, pass)]));
        }
    }

    RGPassBuilder::RGPassBuilder(RenderGraph& inGraph, RGPass& inPass)
        : graph(inGraph)
        , pass(inPass)
    {
    }

    RGPassBuilder::~RGPassBuilder() = default;

    void RGPassBuilder::MarkAsConsumed(RGResource* res)
    {
        auto type = res->GetType();
        Assert(type != RGResourceType::sampler && type != RGResourceType::bindGroup);
        pass.reads.emplace(res);
    }

    RHI::Device& RGPassBuilder::GetDevice()
    {
        return graph.GetDevice();
    }

    void RGPassBuilder::MarkDependenciesFromBindGroup(RGBindGroup* bindGroup)
    {
        const auto& items = bindGroup->GetDesc().items;
        for (const auto& item : items) {
            const auto& bind = item.second;
            auto type = bind.type;

            if (type == RHI::BindingType::uniformBuffer) {
                pass.reads.emplace(bind.bufferView);
            } else if (type == RHI::BindingType::storageBuffer) {
                pass.writes.emplace(bind.bufferView);
            } else if (type == RHI::BindingType::texture) {
                pass.reads.emplace(bind.textureView);
            } else if (type == RHI::BindingType::storagetTexture) {
                pass.writes.emplace(bind.textureView);
            }
        }
    }

    RGCopyPassBuilder::RGCopyPassBuilder(RenderGraph& inGraph, RGCopyPass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , copyPass(inPass)
    {
    }

    RGCopyPassBuilder::~RGCopyPassBuilder() = default;

    void RGCopyPassBuilder::SetAsyncCopy(bool inAsyncCopy)
    {
        copyPass.isAsyncCopy = inAsyncCopy;
    }

    RGComputePassBuilder::RGComputePassBuilder(RenderGraph& inGraph, RGComputePass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , computePass(inPass)
    {
    }

    RGComputePassBuilder::~RGComputePassBuilder() = default;

    void RGComputePassBuilder::SetAsyncCompute(bool inAsyncCompute)
    {
        computePass.isAsyncCompute = inAsyncCompute;
    }

    RGRasterPassBuilder::RGRasterPassBuilder(RenderGraph& inGraph, RGRasterPass& inPass)
        : RGPassBuilder(inGraph, inPass)
        , rasterPass(inPass)
    {
    }

    RGRasterPassBuilder::~RGRasterPassBuilder() = default;

    void RGRasterPassBuilder::SetPassDesc(const RGRasterPassDesc& inDesc)
    {
        rasterPass.passDesc = inDesc;
    }
}
