//
// Created by 兰俊康 on 2023/3/8.
//


#include <Renderer.h>

using namespace RHI;

namespace Example {
    void Renderer::Initialize()
    {
        InitCamera();
        LoadGLTF();
        CreateInstanceAndSelectGPU();
        RequestDeviceAndFetchQueues();
        CreateSwapChain();
        CreateVertexBuffer();
        CreateIndexBuffer();
        CreateQuadBuffer();
        PrepareOffscreen();
        PrepareUniformBuffers();
        CreateSampler();
        CreateBindGroupLayoutAndPipelineLayout();
        CreateBindGroup();
        CreatePipeline();
        CreateFence();
        CreateCommandBuffer();

        GenerateRenderables();
    }

    void Renderer::RenderFrame()
    {
        PopulateCommandBuffer();
        SubmitCommandBufferAndPresent();
    }

    Renderer::~Renderer()
    {
        graphicsQueue->Wait(fence.Get());
        fence->Wait();

        delete model;

        for (auto* renderable : renderables) {
            delete renderable;
        }
    }

    void Renderer::CreateInstanceAndSelectGPU()
    {
        instance = Instance::GetByType(rhiType);
        gpu = instance->GetGpu(0);
    }

    void Renderer::RequestDeviceAndFetchQueues()
    {
        std::vector<QueueInfo> queueCreateInfos = { {QueueType::GRAPHICS, 1} };
        DeviceCreateInfo createInfo {};
        createInfo.queueCreateInfoNum = static_cast<uint32_t>(queueCreateInfos.size());
        createInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(createInfo);
        graphicsQueue = device->GetQueue(QueueType::GRAPHICS, 0);
    }

    void Renderer::CreateSwapChain()
    {
        static std::vector<PixelFormat> swapChainFormatQualifiers = {
            PixelFormat::RGBA8_UNORM,
            PixelFormat::BGRA8_UNORM
        };

        SurfaceCreateInfo surfaceCreateInfo {};
        surfaceCreateInfo.window = app->GetPlatformWindow();
        surface = device->CreateSurface(surfaceCreateInfo);

        for (auto format : swapChainFormatQualifiers) {
            if (device->CheckSwapChainFormatSupport(surface.Get(), format)) {
                swapChainFormat = format;
                break;
            }
        }
        Assert(swapChainFormat != PixelFormat::MAX);

        SwapChainCreateInfo swapChainCreateInfo {};
        swapChainCreateInfo.format = swapChainFormat;
        swapChainCreateInfo.presentMode = PresentMode::IMMEDIATELY;
        swapChainCreateInfo.textureNum = BACK_BUFFER_COUNT;
        swapChainCreateInfo.extent = {app->width, app->height};
        swapChainCreateInfo.surface = surface.Get();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        swapChain = device->CreateSwapChain(swapChainCreateInfo);

        for (auto i = 0; i < swapChainCreateInfo.textureNum; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            TextureViewCreateInfo viewCreateInfo {};
            viewCreateInfo.dimension = TextureViewDimension::TV_2D;
            viewCreateInfo.baseArrayLayer = 0;
            viewCreateInfo.arrayLayerNum = 1;
            viewCreateInfo.baseMipLevel = 0;
            viewCreateInfo.mipLevelNum = 1;
            viewCreateInfo.aspect = TextureAspect::COLOR;
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(viewCreateInfo);
        }
    }

    void Renderer::CreateVertexBuffer()
    {
        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = model->raw_vertex_buffer.size() * sizeof(Vertex);
        bufferCreateInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        vertexBuffer = device->CreateBuffer(bufferCreateInfo);
        assert(vertexBuffer != nullptr);
        auto* data = vertexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_vertex_buffer.data(), bufferCreateInfo.size);
        vertexBuffer->UnMap();

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = bufferCreateInfo.size;
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        vertexBufferView = vertexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void Renderer::CreateIndexBuffer()
    {
        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = model->raw_index_buffer.size() * sizeof(uint32_t);
        bufferCreateInfo.usages = BufferUsageBits::INDEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        indexBuffer = device->CreateBuffer(bufferCreateInfo);
        assert(indexBuffer != nullptr);
        auto* data = indexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_index_buffer.data(), bufferCreateInfo.size);
        indexBuffer->UnMap();


        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = bufferCreateInfo.size;
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.index.format = IndexFormat::UINT32;
        indexBufferView = indexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void Renderer::CreateQuadBuffer()
    {
        // vertex buffer
        std::vector<QuadVertex> vertices {
            {{-1.0f, -1.0f, 0.f}, {0.f, 0.f}},
            {{1.0f, -1.0f, 0.f}, {1.f, 0.f}},
            {{1.0f, 1.0f, 0.f}, {1.f, 1.f}},
            {{-1.0f, 1.0f, 0.f}, {0.f, 1.f}},
        };

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = vertices.size() * sizeof(QuadVertex);
        bufferCreateInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        quadVertexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (quadVertexBuffer != nullptr) {
            auto* data = quadVertexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            quadVertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        quadVertexBufferView = quadVertexBuffer->CreateBufferView(bufferViewCreateInfo);

        // index buffer
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        bufferCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferCreateInfo.usages = BufferUsageBits::INDEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        quadIndexBuffer = device->CreateBuffer(bufferCreateInfo);
        if (quadIndexBuffer != nullptr) {
            auto* data = quadIndexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            quadIndexBuffer->UnMap();
        }

        bufferViewCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.index.format = IndexFormat::UINT32;
        quadIndexBufferView = quadIndexBuffer->CreateBufferView(bufferViewCreateInfo);
    }

    void Renderer::CreateSampler()
    {
        SamplerCreateInfo samplerCreateInfo {};
        sampler = device->CreateSampler(samplerCreateInfo);
    }

    void Renderer::CreateCommandBuffer()
    {
        commandBuffer = device->CreateCommandBuffer();
    }

    void Renderer::CreateFence()
    {
        fence = device->CreateFence();
    }

    void Renderer::CreateBindGroupLayoutAndPipelineLayout()
    {
        std::vector<BindGroupLayoutEntry> entries;
        BindGroupLayoutCreateInfo createInfo {};
        PipelineLayoutCreateInfo pipelineLayoutCreateInfo {};

        //gBuffer
        entries.resize(1);
        entries[0].binding.type = BindingType::UNIFORM_BUFFER;
        entries[0].shaderVisibility = ShaderStageBits::S_VERTEX | ShaderStageBits::S_PIXEL;
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.gBuffer = device->CreateBindGroupLayout(createInfo);

        // renderable layout
        entries.resize(2);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[1].binding.type = BindingType::SAMPLER;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 1;
        renderableLayout = device->CreateBindGroupLayout(createInfo);

        std::vector<BindGroupLayout*> gBufferLayouts { bindGroupLayouts.gBuffer.Get(), renderableLayout.Get() };
        pipelineLayoutCreateInfo.bindGroupLayoutNum = 2;
        pipelineLayoutCreateInfo.bindGroupLayouts = gBufferLayouts.data();
        pipelineLayouts.gBuffer = device->CreatePipelineLayout(pipelineLayoutCreateInfo);

        //ssao
        entries.resize(7);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[1].binding.type = BindingType::TEXTURE;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[2].binding.type = BindingType::TEXTURE;
        entries[2].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[3].binding.type = BindingType::SAMPLER;
        entries[3].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[4].binding.type = BindingType::SAMPLER;
        entries[4].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[5].binding.type = BindingType::UNIFORM_BUFFER;
        entries[5].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[6].binding.type = BindingType::UNIFORM_BUFFER;
        entries[6].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 1 };
            entries[2].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 2 };
            entries[3].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
            entries[4].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 1 };
            entries[5].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 0 };
            entries[6].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 1 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
            entries[2].binding.platform.glsl.index = 2;
            entries[3].binding.platform.glsl.index = 3;
            entries[4].binding.platform.glsl.index = 4;
            entries[5].binding.platform.glsl.index = 5;
            entries[6].binding.platform.glsl.index = 6;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.ssao = device->CreateBindGroupLayout(createInfo);

        std::vector<BindGroupLayout*> ssaoLayouts { bindGroupLayouts.ssao.Get() };
        pipelineLayoutCreateInfo.bindGroupLayoutNum = 1;
        pipelineLayoutCreateInfo.bindGroupLayouts = ssaoLayouts.data();
        pipelineLayouts.ssao = device->CreatePipelineLayout(pipelineLayoutCreateInfo);

        // ssaoBlur
        entries.resize(2);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[1].binding.type = BindingType::SAMPLER;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.ssaoBlur = device->CreateBindGroupLayout(createInfo);

        std::vector<BindGroupLayout*> blurLayouts { bindGroupLayouts.ssaoBlur.Get() };
        pipelineLayoutCreateInfo.bindGroupLayouts = blurLayouts.data();
        pipelineLayouts.ssaoBlur = device->CreatePipelineLayout(pipelineLayoutCreateInfo);

        // composition
        entries.resize(7);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[1].binding.type = BindingType::TEXTURE;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[2].binding.type = BindingType::TEXTURE;
        entries[2].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[3].binding.type = BindingType::TEXTURE;
        entries[3].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[4].binding.type = BindingType::TEXTURE;
        entries[4].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[5].binding.type = BindingType::SAMPLER;
        entries[5].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        entries[6].binding.type = BindingType::UNIFORM_BUFFER;
        entries[6].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::S_PIXEL);
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 1 };
            entries[2].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 2 };
            entries[3].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 3 };
            entries[4].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 4 };
            entries[5].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
            entries[6].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
            entries[2].binding.platform.glsl.index = 2;
            entries[3].binding.platform.glsl.index = 3;
            entries[4].binding.platform.glsl.index = 4;
            entries[5].binding.platform.glsl.index = 5;
            entries[6].binding.platform.glsl.index = 6;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.composition = device->CreateBindGroupLayout(createInfo);

        std::vector<BindGroupLayout*> comLayouts { bindGroupLayouts.composition.Get() };
        pipelineLayoutCreateInfo.bindGroupLayouts = comLayouts.data();
        pipelineLayouts.composition = device->CreatePipelineLayout(pipelineLayoutCreateInfo);
    }

    void Renderer::CreateBindGroup()
    {
        BindGroupCreateInfo createInfo {};
        std::vector<BindGroupEntry> entries(1);

        // GBuffer scene
        entries[0].binding.type = BindingType::UNIFORM_BUFFER;
        entries[0].bufferView = uniformBuffers.sceneParams.bufView.Get();
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = bindGroupLayouts.gBuffer.Get();
        bindGroups.scene = device->CreateBindGroup(createInfo);

        // ssao generation
        entries.resize(7);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].textureView = gBufferPos.view.Get();
        entries[1].binding.type = BindingType::TEXTURE;
        entries[1].textureView = gBufferNormal.view.Get();
        entries[2].binding.type = BindingType::TEXTURE;
        entries[2].textureView = noise.view.Get();
        entries[3].binding.type = BindingType::SAMPLER;
        entries[3].sampler = sampler.Get();
        entries[4].binding.type = BindingType::SAMPLER;
        entries[4].sampler = noiseSampler.Get();
        entries[5].binding.type = BindingType::UNIFORM_BUFFER;
        entries[5].bufferView = uniformBuffers.ssaoKernel.bufView.Get();
        entries[6].binding.type = BindingType::UNIFORM_BUFFER;
        entries[6].bufferView = uniformBuffers.ssaoParams.bufView.Get();
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 1 };
            entries[2].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 2 };
            entries[3].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
            entries[4].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 1 };
            entries[5].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 0 };
            entries[6].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 1 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
            entries[2].binding.platform.glsl.index = 2;
            entries[3].binding.platform.glsl.index = 3;
            entries[4].binding.platform.glsl.index = 4;
            entries[5].binding.platform.glsl.index = 5;
            entries[6].binding.platform.glsl.index = 6;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = bindGroupLayouts.ssao.Get();
        bindGroups.ssao = device->CreateBindGroup(createInfo);

        // ssao blur
        entries.resize(2);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].textureView = ssaoOutput.view.Get();
        entries[1].binding.type = BindingType::SAMPLER;
        entries[1].sampler = sampler.Get();
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = bindGroupLayouts.ssaoBlur.Get();
        bindGroups.ssaoBlur = device->CreateBindGroup(createInfo);

        // composition
        entries.resize(7);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].textureView = gBufferPos.view.Get();
        entries[1].binding.type = BindingType::TEXTURE;
        entries[1].textureView = gBufferNormal.view.Get();
        entries[2].binding.type = BindingType::TEXTURE;
        entries[2].textureView = gBufferAlbedo.view.Get();
        entries[3].binding.type = BindingType::TEXTURE;
        entries[3].textureView = ssaoOutput.view.Get();
        entries[4].binding.type = BindingType::TEXTURE;
        entries[4].textureView = ssaoBlurOutput.view.Get();
        entries[5].binding.type = BindingType::SAMPLER;
        entries[5].sampler = sampler.Get();
        entries[6].binding.type = BindingType::UNIFORM_BUFFER;
        entries[6].bufferView = uniformBuffers.ssaoParams.bufView.Get();
        if (instance->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 1 };
            entries[2].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 2 };
            entries[3].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 3 };
            entries[4].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 4 };
            entries[5].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
            entries[6].binding.platform.hlsl = { HlslBindingRangeType::CONSTANT_BUFFER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
            entries[2].binding.platform.glsl.index = 2;
            entries[3].binding.platform.glsl.index = 3;
            entries[4].binding.platform.glsl.index = 4;
            entries[5].binding.platform.glsl.index = 5;
            entries[6].binding.platform.glsl.index = 6;
        }
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = bindGroupLayouts.composition.Get();
        bindGroups.composition = device->CreateBindGroup(createInfo);
    }

    void Renderer::PrepareOffscreen()
    {
        CreateAttachments(PixelFormat::RGBA32_FLOAT, TextureUsageBits::TEXTURE_BINDING | TextureUsageBits::RENDER_ATTACHMENT, TextureAspect::COLOR, &gBufferPos, app->width, app->height);
        CreateAttachments(PixelFormat::RGBA8_UNORM, TextureUsageBits::TEXTURE_BINDING | TextureUsageBits::RENDER_ATTACHMENT, TextureAspect::COLOR, &gBufferNormal, app->width, app->height);
        CreateAttachments(PixelFormat::RGBA8_UNORM, TextureUsageBits::TEXTURE_BINDING | TextureUsageBits::RENDER_ATTACHMENT, TextureAspect::COLOR, &gBufferAlbedo, app->width, app->height);
        CreateAttachments(PixelFormat::D32_FLOAT_S8_UINT, TextureUsageBits::DEPTH_STENCIL_ATTACHMENT, TextureAspect::DEPTH_STENCIL, &gBufferDepth, app->width, app->height);

        CreateAttachments(PixelFormat::R8_UNORM, TextureUsageBits::TEXTURE_BINDING | TextureUsageBits::RENDER_ATTACHMENT, TextureAspect::COLOR, &ssaoOutput, app->width, app->height);

        CreateAttachments(PixelFormat::R8_UNORM, TextureUsageBits::TEXTURE_BINDING | TextureUsageBits::RENDER_ATTACHMENT, TextureAspect::COLOR, &ssaoBlurOutput, app->width, app->height);
    }

    void Renderer::PrepareUniformBuffers()
    {
        // scene matries
        uboSceneParams.projection = camera.perspective;
        uboSceneParams.view = camera.view;
        uboSceneParams.model = glm::mat4(1.0f);
        CreateUniformBuffer(BufferUsageBits::UNIFORM | BufferUsageBits::MAP_WRITE, &uniformBuffers.sceneParams, sizeof(UBOSceneParams), &uboSceneParams);

        // ssao parameters
        ubossaoParams.projection = camera.perspective;
        CreateUniformBuffer(BufferUsageBits::UNIFORM | BufferUsageBits::MAP_WRITE, &uniformBuffers.ssaoParams, sizeof(UBOSSAOParams), &ubossaoParams);

        // ssao kennel
        std::default_random_engine rndEngine((unsigned)time(nullptr));
        std::uniform_real_distribution<float> rndDist(0.0f, 1.0f);
        std::vector<glm::vec4> ssaoKernel(SSAO_KERNEL_SIZE);

        auto lerp = [](float a, float b, float f) ->float {
            return a + f * (b - a);
        };

        for (uint32_t i = 0; i < SSAO_KERNEL_SIZE; ++i)
        {
            glm::vec3 sample(rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine) * 2.0 - 1.0, rndDist(rndEngine));
            sample = glm::normalize(sample);
            sample *= rndDist(rndEngine);
            float scale = float(i) / float(SSAO_KERNEL_SIZE);
            scale = lerp(0.1f, 1.0f, scale * scale);
            ssaoKernel[i] = glm::vec4(sample * scale, 0.0f);
        }
        CreateUniformBuffer(BufferUsageBits::UNIFORM | BufferUsageBits::MAP_WRITE, &uniformBuffers.ssaoKernel, ssaoKernel.size() * sizeof(glm::vec4), ssaoKernel.data());

        // random noise
        std::vector<glm::vec4> ssaoNoise(SSAO_NOISE_DIM * SSAO_NOISE_DIM);
        for (auto& randomVec : ssaoNoise)
        {
            randomVec = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
        }

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = ssaoNoise.size() * sizeof(glm::vec4);
        // To make this buffer has correct resource state(D3D12_RESOURCE_STATE_GENERIC_READ) in dx, add uniform usage flag
        bufferCreateInfo.usages = BufferUsageBits::UNIFORM | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        auto* pixelBuffer = device->CreateBuffer(bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* data = pixelBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, ssaoNoise.data(), bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }

        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = PixelFormat::RGBA32_FLOAT;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {SSAO_NOISE_DIM, SSAO_NOISE_DIM, 1};
        texCreateInfo.dimension = TextureDimension::T_2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::COPY_DST | TextureUsageBits::TEXTURE_BINDING;
        noise.tex = device->CreateTexture(texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::TV_2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::COLOR;
        noise.view = noise.tex->CreateTextureView(viewCreateInfo);

        SamplerCreateInfo samplerCreateInfo {};
        samplerCreateInfo.addressModeU = AddressMode::REPEAT;
        samplerCreateInfo.addressModeV = AddressMode::REPEAT;
        noiseSampler = device->CreateSampler(samplerCreateInfo);

        auto* texCommandBuffer = device->CreateCommandBuffer();
        auto* commandEncoder = texCommandBuffer->Begin();
        // Dx need not to transition resource state before copy
        commandEncoder->ResourceBarrier(Barrier::Transition(noise.tex.Get(), TextureState::UNDEFINED, TextureState::COPY_DST));
        TextureSubResourceInfo subResourceInfo {};
        subResourceInfo.mipLevel = 0;
        subResourceInfo.arrayLayerNum = 1;
        subResourceInfo.baseArrayLayer = 0;
        subResourceInfo.aspect = TextureAspect::COLOR;
        commandEncoder->CopyBufferToTexture(pixelBuffer, noise.tex.Get(), &subResourceInfo, {SSAO_NOISE_DIM, SSAO_NOISE_DIM, 1});
        commandEncoder->ResourceBarrier(Barrier::Transition(noise.tex.Get(), TextureState::COPY_DST, TextureState::SHADER_READ_ONLY));
        commandEncoder->End();

        graphicsQueue->Submit(texCommandBuffer, nullptr);

    }

    void Renderer::CreateAttachments(RHI::PixelFormat format, TextureUsageFlags flags, TextureAspect aspect, ColorAttachment* attachment, uint32_t width, uint32_t height)
    {
        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = format;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {width, height, 1};
        texCreateInfo.dimension = TextureDimension::T_2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = flags;
        attachment->texture = device->CreateTexture(texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::TV_2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = aspect;
        attachment->view = attachment->texture->CreateTextureView(viewCreateInfo);
    }

    ShaderModule* Renderer::CompileShader(const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage)
    {
        std::string shaderSource = Common::FileUtils::ReadTextFile(fileName);

        Render::ShaderCompileInput info;
        info.source = shaderSource;
        info.entryPoint = entryPoint;
        info.stage = shaderStage;
        Render::ShaderCompileOptions options;
        options.includePaths.emplace_back("shaders");
        if (rhiType == RHI::RHIType::DIRECTX_12) {
            options.byteCodeType = Render::ShaderByteCodeType::DXIL;
        } else if (rhiType == RHI::RHIType::VULKAN) {
            options.byteCodeType = Render::ShaderByteCodeType::SPRIV;
        } else if (rhiType == RHI::RHIType::METAL) {
            options.byteCodeType = Render::ShaderByteCodeType::MBC;
        }
        options.withDebugInfo = false;
        auto future = Render::ShaderCompiler::Get().Compile(info, options);

        future.wait();
        auto result = future.get();
        if (!result.success) {
            std::cout << "failed to compiler shader (" << fileName << ", " << info.entryPoint << ")" << std::endl << result.errorInfo << std::endl;
        }
        Assert(result.success);

        ShaderModuleCreateInfo createInfo {};
        createInfo.size = result.byteCode.size();
        createInfo.byteCode = result.byteCode.data();

        return device->CreateShaderModule(createInfo);
    }

    void Renderer::CreateUniformBuffer(RHI::BufferUsageFlags flags, UBuffer* uBuffer, size_t size, void* data)
    {
        BufferCreateInfo createInfo {};
        createInfo.size = size;
        createInfo.usages = flags;

        uBuffer->buf = device->CreateBuffer(createInfo);
        if (uBuffer->buf != nullptr && data != nullptr) {
            auto* mapData = uBuffer->buf->Map(MapMode::WRITE, 0, size);
            memcpy(mapData, data, size);
            uBuffer->buf->UnMap();
        }

        BufferViewCreateInfo viewCreateInfo {};
        viewCreateInfo.size = size;
        viewCreateInfo.offset = 0;
        uBuffer->bufView = uBuffer->buf->CreateBufferView(viewCreateInfo);
    }

    void Renderer::CreatePipeline()
    {
        shaderModules.gBufferVert     = CompileShader("shaders/gbufferVs.hlsl", "VSMain", ShaderStageBits::S_VERTEX);
        shaderModules.gBufferFrag     = CompileShader("shaders/gbufferPs.hlsl", "FSMain", ShaderStageBits::S_PIXEL);
        shaderModules.quadVert        = CompileShader("shaders/fullscreenVs.hlsl", "VSMain", ShaderStageBits::S_VERTEX);
        shaderModules.ssaoFrag        = CompileShader("shaders/ssaoPs.hlsl", "FSMain", ShaderStageBits::S_PIXEL);
        shaderModules.ssaoBlurFrag    = CompileShader("shaders/blurPs.hlsl", "FSMain", ShaderStageBits::S_PIXEL);
        shaderModules.compositionFrag = CompileShader("shaders/compositionPs.hlsl", "FSMain", ShaderStageBits::S_PIXEL);

        // Gbuffer vertex
        std::array<VertexAttribute, 4> vertexAttributes {};
        vertexAttributes[0].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[0].offset = 0;
        vertexAttributes[0].semanticName = "POSITION";
        vertexAttributes[0].semanticIndex = 0;
        vertexAttributes[1].format = VertexFormat::FLOAT32_X2;
        vertexAttributes[1].offset = offsetof(Vertex, uv);
        vertexAttributes[1].semanticName = "TEXCOORD";
        vertexAttributes[1].semanticIndex = 0;
        vertexAttributes[2].format = VertexFormat::FLOAT32_X4;
        vertexAttributes[2].offset = offsetof(Vertex, color);
        vertexAttributes[2].semanticName = "COLOR";
        vertexAttributes[2].semanticIndex = 0;
        vertexAttributes[3].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[3].offset = offsetof(Vertex, normal);
        vertexAttributes[3].semanticName = "NORMAL";
        vertexAttributes[3].semanticIndex = 0;

        VertexBufferLayout vertexBufferLayout {};
        vertexBufferLayout.stepMode = RHI::VertexStepMode::PER_VERTEX;
        vertexBufferLayout.stride = sizeof(Vertex);
        vertexBufferLayout.attributeNum = vertexAttributes.size();
        vertexBufferLayout.attributes = vertexAttributes.data();

        // quad buffer vertex
        std::array<VertexAttribute, 2> quadVertexAttributes {};
        quadVertexAttributes[0].format = VertexFormat::FLOAT32_X3;
        quadVertexAttributes[0].offset = 0;
        quadVertexAttributes[0].semanticName = "POSITION";
        quadVertexAttributes[0].semanticIndex = 0;
        quadVertexAttributes[1].format = VertexFormat::FLOAT32_X2;
        quadVertexAttributes[1].offset = offsetof(QuadVertex, uv);
        quadVertexAttributes[1].semanticName = "TEXCOORD";
        quadVertexAttributes[1].semanticIndex = 0;

        VertexBufferLayout quadVertexBufferLayout {};
        quadVertexBufferLayout.stepMode = RHI::VertexStepMode::PER_VERTEX;
        quadVertexBufferLayout.stride = sizeof(QuadVertex);
        quadVertexBufferLayout.attributeNum = quadVertexAttributes.size();
        quadVertexBufferLayout.attributes = quadVertexAttributes.data();

        // General pipeline infos
        GraphicsPipelineCreateInfo createInfo {};
        createInfo.vertexState.bufferLayoutNum = 1;
        createInfo.primitiveState.depthClip = false;
        createInfo.primitiveState.frontFace = RHI::FrontFace::CCW;
        createInfo.primitiveState.cullMode = CullMode::NONE;
        createInfo.primitiveState.topologyType = RHI::PrimitiveTopologyType::TRIANGLE;
        createInfo.primitiveState.stripIndexFormat = IndexFormat::UINT32;
        createInfo.multiSampleState.count = 1;

        // Gbuffer
        {
            DepthStencilState depthStencilState {};
            depthStencilState.depthEnable = true;
            depthStencilState.depthComparisonFunc = ComparisonFunc::LESS_EQUAL;
            depthStencilState.format = PixelFormat::D32_FLOAT_S8_UINT;
            
            std::array<ColorTargetState, 3> colorTargetStates {};
            colorTargetStates[0].format = PixelFormat::RGBA32_FLOAT;
            colorTargetStates[0].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;
            colorTargetStates[1].format = PixelFormat::RGBA8_UNORM;
            colorTargetStates[1].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;
            colorTargetStates[2].format = PixelFormat::RGBA8_UNORM;
            colorTargetStates[2].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

            createInfo.depthStencilState = depthStencilState;
            createInfo.vertexState.bufferLayouts = &vertexBufferLayout;
            createInfo.fragmentState.colorTargetNum = colorTargetStates.size();
            createInfo.fragmentState.colorTargets = colorTargetStates.data();
            createInfo.vertexShader = shaderModules.gBufferVert.Get();
            createInfo.pixelShader = shaderModules.gBufferFrag.Get();
            createInfo.layout = pipelineLayouts.gBuffer.Get();
            pipelines.gBuffer = device->CreateGraphicsPipeline(createInfo);
        }

        // ssao
        {
            ColorTargetState colorTargetState {};
            colorTargetState.format = PixelFormat::R8_UNORM;
            colorTargetState.writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;
            
            DepthStencilState depthStencilState {};

            createInfo.depthStencilState = depthStencilState;
            createInfo.vertexState.bufferLayouts = &quadVertexBufferLayout;
            createInfo.fragmentState.colorTargetNum = 1;
            createInfo.fragmentState.colorTargets = &colorTargetState;
            createInfo.vertexShader = shaderModules.quadVert.Get();
            createInfo.pixelShader = shaderModules.ssaoFrag.Get();
            createInfo.layout = pipelineLayouts.ssao.Get();
            pipelines.ssao = device->CreateGraphicsPipeline(createInfo);
        }

        // ssaoBlur
        {
            ColorTargetState colorTargetState {};
            colorTargetState.format = PixelFormat::R8_UNORM;
            colorTargetState.writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

            DepthStencilState depthStencilState {};

            createInfo.depthStencilState = depthStencilState;
            createInfo.vertexState.bufferLayouts = &quadVertexBufferLayout;
            createInfo.fragmentState.colorTargetNum = 1;
            createInfo.fragmentState.colorTargets = &colorTargetState;
            createInfo.vertexShader = shaderModules.quadVert.Get();
            createInfo.pixelShader = shaderModules.ssaoBlurFrag.Get();
            createInfo.layout = pipelineLayouts.ssaoBlur.Get();
            pipelines.ssaoBlur = device->CreateGraphicsPipeline(createInfo);
        }

        // composition
        {
            ColorTargetState colorTargetState {};
            colorTargetState.format = swapChainFormat;
            colorTargetState.writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;
            
            DepthStencilState depthStencilState {};
            
            createInfo.depthStencilState = depthStencilState;
            createInfo.vertexState.bufferLayouts = &quadVertexBufferLayout;
            createInfo.fragmentState.colorTargetNum = 1;
            createInfo.fragmentState.colorTargets = &colorTargetState;
            createInfo.vertexShader = shaderModules.quadVert.Get();
            createInfo.pixelShader = shaderModules.compositionFrag.Get();
            createInfo.layout = pipelineLayouts.composition.Get();
            pipelines.composition = device->CreateGraphicsPipeline(createInfo);
        }
    }

    void Renderer::PopulateCommandBuffer()
    {
        CommandEncoder* commandEncoder = commandBuffer->Begin();
        {
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferPos.texture.Get(), TextureState::UNDEFINED, TextureState::RENDER_TARGET));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferNormal.texture.Get(), TextureState::UNDEFINED, TextureState::RENDER_TARGET));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferAlbedo.texture.Get(), TextureState::UNDEFINED, TextureState::RENDER_TARGET));

            std::array<GraphicsPassColorAttachment, 3> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = gBufferPos.view.Get();
            colorAttachments[0].resolve = nullptr;
            colorAttachments[1].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[1].loadOp = LoadOp::CLEAR;
            colorAttachments[1].storeOp = StoreOp::STORE;
            colorAttachments[1].view = gBufferNormal.view.Get();
            colorAttachments[1].resolve = nullptr;
            colorAttachments[2].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[2].loadOp = LoadOp::CLEAR;
            colorAttachments[2].storeOp = StoreOp::STORE;
            colorAttachments[2].view = gBufferAlbedo.view.Get();
            colorAttachments[2].resolve = nullptr;
            
            GraphicsPassDepthStencilAttachment depthAttachment {};
            depthAttachment.view = gBufferDepth.view.Get();
            depthAttachment.depthLoadOp = LoadOp::CLEAR;
            depthAttachment.depthStoreOp = StoreOp::STORE;
            depthAttachment.depthReadOnly = true;
            depthAttachment.depthClearValue = 1.0;
            depthAttachment.stencilClearValue = 0.0;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = &depthAttachment;

            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetPipeline(pipelines.gBuffer.Get());
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.scene.Get());
                graphicsEncoder->SetVertexBuffer(0, vertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(indexBufferView.Get());

                for (auto* renderable : renderables) {
                    graphicsEncoder->SetBindGroup(1, renderable->bindGroup.Get());
                    graphicsEncoder->DrawIndexed(renderable->indexCount, 1, renderable->firstIndex, 0, 0);
                }
            }
            graphicsEncoder->EndPass();

            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferPos.texture.Get(), TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferNormal.texture.Get(), TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferAlbedo.texture.Get(), TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
        }

        {
            // ssao
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoOutput.texture.Get(), TextureState::UNDEFINED, TextureState::RENDER_TARGET));

            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = ssaoOutput.view.Get();
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetPipeline(pipelines.ssao.Get());
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.ssao.Get());
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView.Get());
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoOutput.texture.Get(), TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
        }

        {
            // ssaoBlur
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoBlurOutput.texture.Get(), TextureState::UNDEFINED, TextureState::RENDER_TARGET));

            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = ssaoBlurOutput.view.Get();
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetPipeline(pipelines.ssaoBlur.Get());
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.ssaoBlur.Get());
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView.Get());
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoBlurOutput.texture.Get(), TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
        }

        {
            auto backTextureIndex = swapChain->AcquireBackTexture();

            // composition
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::PRESENT, TextureState::RENDER_TARGET));

            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = swapChainTextureViews[backTextureIndex].Get();
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::PRESENT, TextureState::RENDER_TARGET));
            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetPipeline(pipelines.composition.Get());
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.composition.Get());
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView.Get());
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView.Get());
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::RENDER_TARGET, TextureState::PRESENT));
        }

        commandEncoder->SwapChainSync(swapChain.Get());
        commandEncoder->End();
    }

    void Renderer::SubmitCommandBufferAndPresent()
    {
        graphicsQueue->Submit(commandBuffer.Get(), fence.Get());
        fence->Wait();
        swapChain->Present();
    }

    void Renderer::InitCamera()
    {
        camera.type = Camera::CameraType::firstPerson;
        camera.position = { 4.0f, 2.4f, -2.4f };
        camera.setRotation(glm::vec3(.0f, 58.0f, 0.0f));
        camera.setPerspective(60.0f, (float)app->width / (float)app->height, uboSceneParams.nearPlane, uboSceneParams.farPlane);
    }

    void Renderer::LoadGLTF()
    {
        model = new Model();
        model->LoadFromFile("models/voyager.gltf");
    }

    Renderable::~Renderable()
    {
    }

    void Renderer::GenerateRenderables()
    {
        for (auto* node : model->linearNodes) {
            for (auto* primitive : node->meshes) {
                auto* renderable = new Renderable();
                renderable->InitalizeWithPrimitive(this, primitive);

                renderables.emplace_back(renderable);
            }
        }
    }

    void Renderable::InitalizeWithPrimitive(Renderer* renderer ,Mesh* primitive)
    {
        indexCount = primitive->indexCount;
        firstIndex = primitive->firstIndex;
        
        // upload diffuseColorMap
        auto* texData = primitive->materialData->baseColorTexture;

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = texData->GetSize();
        bufferCreateInfo.usages = BufferUsageBits::UNIFORM | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        auto* pixelBuffer = renderer->GetDevice()->CreateBuffer(bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* mapData = pixelBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(mapData, texData->buffer.data(), bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }

        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = PixelFormat::RGBA8_UNORM;
        texCreateInfo.dimension = TextureDimension::T_2D;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = { texData->width, texData->height, 1};
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::COPY_DST | TextureUsageBits::TEXTURE_BINDING;
        diffuseColorMap = renderer->GetDevice()->CreateTexture(texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::TV_2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::COLOR;
        diffuseColorMapView = diffuseColorMap->CreateTextureView(viewCreateInfo);

        auto* texCommandBuffer = renderer->GetDevice()->CreateCommandBuffer();
        auto* commandEncoder = texCommandBuffer->Begin();
        // Dx need not to transition resource state before copy
        commandEncoder->ResourceBarrier(Barrier::Transition(diffuseColorMap.Get(), TextureState::UNDEFINED, TextureState::COPY_DST));
        TextureSubResourceInfo subResourceInfo {};
        subResourceInfo.mipLevel = 0;
        subResourceInfo.arrayLayerNum = 1;
        subResourceInfo.baseArrayLayer = 0;
        subResourceInfo.aspect = TextureAspect::COLOR;
        commandEncoder->CopyBufferToTexture(pixelBuffer, diffuseColorMap.Get(), &subResourceInfo, {texData->width, texData->height, 1});
        commandEncoder->ResourceBarrier(Barrier::Transition(diffuseColorMap.Get(), TextureState::COPY_DST, TextureState::SHADER_READ_ONLY));
        commandEncoder->End();

        renderer->GetQueue()->Submit(texCommandBuffer, nullptr);

        // per renderable bindGroup
        std::vector<BindGroupEntry> entries(2);
        entries[0].binding.type = BindingType::TEXTURE;
        entries[0].textureView = diffuseColorMapView.Get();
        entries[1].binding.type = BindingType::SAMPLER;
        entries[1].sampler = renderer->GetSampler();
        if (renderer->GetInstance()->GetRHIType() == RHI::RHIType::DIRECTX_12) {
            entries[0].binding.platform.hlsl = { HlslBindingRangeType::TEXTURE, 0 };
            entries[1].binding.platform.hlsl = { HlslBindingRangeType::SAMPLER, 0 };
        } else {
            entries[0].binding.platform.glsl.index = 0;
            entries[1].binding.platform.glsl.index = 1;
        }

        BindGroupCreateInfo createInfo {};
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = renderer->GetLayout();

        bindGroup = renderer->GetDevice()->CreateBindGroup(createInfo);
    }
}