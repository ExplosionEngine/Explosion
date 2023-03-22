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
//        PopulateCommandBuffer();
//        SubmitCommandBufferAndPresent();
    }

    Renderer::~Renderer()
    {
        graphicsQueue->Wait(fence);
        fence->Wait();
        fence->Destroy();
        commandBuffer->Destroy();
        for (auto* textureView: swapChainTextureViews) {
            textureView->Destroy();
        }
        vertexBufferView->Destroy();
        vertexBuffer->Destroy();
        indexBuffer->Destroy();
        indexBufferView->Destroy();

        quadVertexBuffer->Destroy();
        quadVertexBufferView->Destroy();
        quadIndexBuffer->Destroy();
        quadIndexBufferView->Destroy();

        sampler->Destroy();
        noiseSampler->Destroy();

        delete model;

        swapChain->Destroy();
        device->Destroy();

    }

    void Renderer::CreateInstanceAndSelectGPU()
    {
        instance = Instance::CreateByType(rhiType);
        gpu = instance->GetGpu(0);
    }

    void Renderer::RequestDeviceAndFetchQueues()
    {
        std::vector<QueueInfo> queueCreateInfos = { {QueueType::GRAPHICS, 1} };
        DeviceCreateInfo createInfo {};
        createInfo.queueCreateInfoNum = queueCreateInfos.size();
        createInfo.queueCreateInfos = queueCreateInfos.data();
        device = gpu->RequestDevice(&createInfo);
        graphicsQueue = device->GetQueue(QueueType::GRAPHICS, 0);
    }

    void Renderer::CreateSwapChain()
    {
        SwapChainCreateInfo swapChainCreateInfo {};
        swapChainCreateInfo.format = PixelFormat::RGBA8_UNORM;
        swapChainCreateInfo.presentMode = PresentMode::IMMEDIATELY;
        swapChainCreateInfo.textureNum = BACK_BUFFER_COUNT;
        swapChainCreateInfo.extent = {app->width, app->height};
        swapChainCreateInfo.window = app->GetPlatformWindow();
        swapChainCreateInfo.presentQueue = graphicsQueue;
        swapChain = device->CreateSwapChain(&swapChainCreateInfo);

        for (auto i = 0; i < swapChainCreateInfo.textureNum; i++) {
            swapChainTextures[i] = swapChain->GetTexture(i);

            TextureViewCreateInfo viewCreateInfo {};
            viewCreateInfo.dimension = TextureViewDimension::TV_2D;
            viewCreateInfo.baseArrayLayer = 0;
            viewCreateInfo.arrayLayerNum = 1;
            viewCreateInfo.baseMipLevel = 0;
            viewCreateInfo.mipLevelNum = 1;
            viewCreateInfo.aspect = TextureAspect::COLOR;
            swapChainTextureViews[i] = swapChainTextures[i]->CreateTextureView(&viewCreateInfo);
        }
    }

    void Renderer::CreateVertexBuffer()
    {
        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = model->raw_vertex_buffer.size() * sizeof(Vertex);
        bufferCreateInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        vertexBuffer = device->CreateBuffer(&bufferCreateInfo);
        assert(vertexBuffer != nullptr);
        auto* data = vertexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_vertex_buffer.data(), bufferCreateInfo.size);
        vertexBuffer->UnMap();

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = bufferCreateInfo.size;
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        vertexBufferView = vertexBuffer->CreateBufferView(&bufferViewCreateInfo);
    }

    void Renderer::CreateIndexBuffer()
    {
        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = model->raw_index_buffer.size() * sizeof(uint32_t);
        bufferCreateInfo.usages = BufferUsageBits::INDEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        indexBuffer = device->CreateBuffer(&bufferCreateInfo);
        assert(indexBuffer != nullptr);
        auto* data = indexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
        memcpy(data, model->raw_index_buffer.data(), bufferCreateInfo.size);
        indexBuffer->UnMap();


        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = bufferCreateInfo.size;
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.index.format = IndexFormat::UINT32;
        indexBufferView = indexBuffer->CreateBufferView(&bufferViewCreateInfo);
    }

    void Renderer::CreateQuadBuffer()
    {
        // vertex buffer
        std::vector<QuadVertex> vertices {
            {{-.5f, -.5f, 0.f}, {0.f, 0.f}},
            {{.5f, -.5f, 0.f}, {1.f, 0.f}},
            {{.5f, .5f, 0.f}, {1.f, 1.f}},
            {{-.5f, .5f, 0.f}, {0.f, 1.f}},
        };

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = vertices.size() * sizeof(QuadVertex);
        bufferCreateInfo.usages = BufferUsageBits::VERTEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        quadVertexBuffer = device->CreateBuffer(&bufferCreateInfo);
        if (quadVertexBuffer != nullptr) {
            auto* data = quadVertexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, vertices.data(), bufferCreateInfo.size);
            quadVertexBuffer->UnMap();
        }

        BufferViewCreateInfo bufferViewCreateInfo {};
        bufferViewCreateInfo.size = vertices.size() * sizeof(Vertex);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.vertex.stride = sizeof(Vertex);
        quadVertexBufferView = quadVertexBuffer->CreateBufferView(&bufferViewCreateInfo);

        // index buffer
        std::vector<uint32_t> indices = {0, 1, 2, 0, 2, 3};
        bufferCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferCreateInfo.usages = BufferUsageBits::INDEX | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        quadIndexBuffer = device->CreateBuffer(&bufferCreateInfo);
        if (quadIndexBuffer != nullptr) {
            auto* data = quadIndexBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(data, indices.data(), bufferCreateInfo.size);
            quadIndexBuffer->UnMap();
        }

        bufferViewCreateInfo.size = indices.size() * sizeof(uint32_t);
        bufferViewCreateInfo.offset = 0;
        bufferViewCreateInfo.index.format = IndexFormat::UINT32;
        quadIndexBufferView = quadIndexBuffer->CreateBufferView(&bufferViewCreateInfo);
    }

    void Renderer::CreateSampler()
    {
        SamplerCreateInfo samplerCreateInfo {};
        sampler = device->CreateSampler(&samplerCreateInfo);
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
        entries[0].type = BindingType::UNIFORM_BUFFER;
        entries[0].binding = 0;
        entries[0].shaderVisibility = ShaderStageBits::VERTEX | ShaderStageBits::FRAGMENT;
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.gBuffer = device->CreateBindGroupLayout(&createInfo);

        // renderable layout
        entries.resize(2);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[1].type = BindingType::SAMPLER;
        entries[1].binding = 1;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 1;
        renderableLayout = device->CreateBindGroupLayout(&createInfo);

        std::vector<BindGroupLayout*> layouts {bindGroupLayouts.gBuffer, renderableLayout};
        pipelineLayoutCreateInfo.bindGroupNum = 2;
        pipelineLayoutCreateInfo.bindGroupLayouts = layouts.data();
        pipelineLayouts.gBuffer = device->CreatePipelineLayout(&pipelineLayoutCreateInfo);

        //ssao
        entries.resize(7);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[1].type = BindingType::TEXTURE;
        entries[1].binding = 1;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[2].type = BindingType::TEXTURE;
        entries[2].binding = 2;
        entries[2].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[3].type = BindingType::SAMPLER;
        entries[3].binding = 3;
        entries[3].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[4].type = BindingType::SAMPLER;
        entries[4].binding = 4;
        entries[4].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[5].type = BindingType::UNIFORM_BUFFER;
        entries[5].binding = 5;
        entries[5].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[6].type = BindingType::UNIFORM_BUFFER;
        entries[6].binding = 6;
        entries[6].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.ssao = device->CreateBindGroupLayout(&createInfo);

        pipelineLayoutCreateInfo.bindGroupNum = 1;
        pipelineLayoutCreateInfo.bindGroupLayouts = &bindGroupLayouts.ssao;
        pipelineLayouts.ssao = device->CreatePipelineLayout(&pipelineLayoutCreateInfo);

        // ssaoBlur
        entries.resize(2);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[1].type = BindingType::SAMPLER;
        entries[1].binding = 1;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.ssaoBlur = device->CreateBindGroupLayout(&createInfo);

        pipelineLayoutCreateInfo.bindGroupLayouts = &bindGroupLayouts.ssaoBlur;
        pipelineLayouts.ssaoBlur = device->CreatePipelineLayout(&pipelineLayoutCreateInfo);

        // composition
        entries.resize(7);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[1].type = BindingType::TEXTURE;
        entries[1].binding = 1;
        entries[1].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[2].type = BindingType::TEXTURE;
        entries[2].binding = 2;
        entries[2].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[3].type = BindingType::TEXTURE;
        entries[3].binding = 3;
        entries[3].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[4].type = BindingType::TEXTURE;
        entries[4].binding = 4;
        entries[4].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[5].type = BindingType::SAMPLER;
        entries[5].binding = 5;
        entries[5].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        entries[6].type = BindingType::UNIFORM_BUFFER;
        entries[6].binding = 6;
        entries[6].shaderVisibility = static_cast<ShaderStageFlags>(ShaderStageBits::FRAGMENT);
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layoutIndex = 0;
        bindGroupLayouts.composition = device->CreateBindGroupLayout(&createInfo);

        pipelineLayoutCreateInfo.bindGroupLayouts = &bindGroupLayouts.composition;
        pipelineLayouts.composition = device->CreatePipelineLayout(&pipelineLayoutCreateInfo);
    }

    void Renderer::CreateBindGroup()
    {
        BindGroupCreateInfo createInfo {};
        std::vector<BindGroupEntry> entries(1);

        // GBuffer scene
        entries[0].type = BindingType::UNIFORM_BUFFER;
        entries[0].binding = 0;
        entries[0].bufferView = uniformBuffers.sceneParams.bufView;
        createInfo.entries = entries.data();
        createInfo.entryNum = entries.size();
        createInfo.layout = bindGroupLayouts.gBuffer;
        bindGroups.scene = device->CreateBindGroup(&createInfo);

        // ssao generation
        entries.resize(7);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].textureView = gBufferOutput.pos.view;
        entries[1].type = BindingType::TEXTURE;
        entries[1].binding = 1;
        entries[1].textureView = gBufferOutput.normal.view;
        entries[2].type = BindingType::TEXTURE;
        entries[2].binding = 2;
        entries[2].textureView = noise.view;
        entries[3].type = BindingType::SAMPLER;
        entries[3].binding = 3;
        entries[3].sampler = sampler;
        entries[4].type = BindingType::SAMPLER;
        entries[4].binding = 4;
        entries[4].sampler = noiseSampler;
        entries[5].type = BindingType::UNIFORM_BUFFER;
        entries[5].binding = 5;
        entries[5].bufferView = uniformBuffers.ssaoKernel.bufView;
        entries[6].type = BindingType::UNIFORM_BUFFER;
        entries[6].binding = 6;
        entries[6].bufferView = uniformBuffers.ssaoParams.bufView;
        createInfo.entries = entries.data();
        createInfo.entryNum = entries.size();
        createInfo.layout = bindGroupLayouts.ssao;
        bindGroups.ssao = device->CreateBindGroup(&createInfo);

        // ssao blur
        entries.resize(2);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].textureView = ssaoOutput.view;
        entries[1].type = BindingType::SAMPLER;
        entries[1].binding = 1;
        entries[1].sampler = sampler;
        createInfo.entries = entries.data();
        createInfo.entryNum = entries.size();
        createInfo.layout = bindGroupLayouts.ssaoBlur;
        bindGroups.ssaoBlur = device->CreateBindGroup(&createInfo);

        // composition
        entries.resize(7);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].textureView = gBufferOutput.pos.view;
        entries[1].type = BindingType::TEXTURE;
        entries[1].binding = 1;
        entries[1].textureView = gBufferOutput.normal.view;
        entries[2].type = BindingType::TEXTURE;
        entries[2].binding = 2;
        entries[2].textureView = gBufferOutput.albedo.view;
        entries[3].type = BindingType::TEXTURE;
        entries[3].binding = 3;
        entries[3].textureView = ssaoOutput.view;
        entries[4].type = BindingType::TEXTURE;
        entries[4].binding = 4;
        entries[4].textureView = ssaoBlurOutput.view;
        entries[5].type = BindingType::SAMPLER;
        entries[5].binding = 5;
        entries[5].sampler = sampler;
        entries[6].type = BindingType::UNIFORM_BUFFER;
        entries[6].binding = 6;
        entries[6].bufferView = uniformBuffers.ssaoParams.bufView;
        createInfo.entries = entries.data();
        createInfo.entryNum = entries.size();
        createInfo.layout = bindGroupLayouts.composition;
        bindGroups.ssao = device->CreateBindGroup(&createInfo);
    }

    void Renderer::PrepareOffscreen()
    {
        CreateAttachments(PixelFormat::RGBA32_FLOAT, TextureAspect::COLOR, &gBufferOutput.pos, app->width, app->height);
        CreateAttachments(PixelFormat::RGBA8_UNORM, TextureAspect::COLOR, &gBufferOutput.normal, app->width, app->height);
        CreateAttachments(PixelFormat::RGBA8_UNORM, TextureAspect::COLOR, &gBufferOutput.albedo, app->width, app->height);

        CreateAttachments(PixelFormat::R8_UNORM, TextureAspect::COLOR, &ssaoOutput, app->width, app->height);

        CreateAttachments(PixelFormat::R8_UNORM, TextureAspect::COLOR, &ssaoBlurOutput, app->width, app->height);
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
        for (uint32_t i = 0; i < static_cast<uint32_t>(ssaoNoise.size()); i++)
        {
            ssaoNoise[i] = glm::vec4(rndDist(rndEngine) * 2.0f - 1.0f, rndDist(rndEngine) * 2.0f - 1.0f, 0.0f, 0.0f);
        }

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = ssaoNoise.size() * sizeof(glm::vec4);
        // TO make this buffer has correct resource state(D3D12_RESOURCE_STATE_GENERIC_READ) in dx, add uniform usage flag
        bufferCreateInfo.usages = BufferUsageBits::UNIFORM | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        auto* pixelBuffer = device->CreateBuffer(&bufferCreateInfo);
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
        noise.tex = device->CreateTexture(&texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::TV_2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::COLOR;
        noise.view = noise.tex->CreateTextureView(&viewCreateInfo);

        // use the default attrib to create sampler
        SamplerCreateInfo samplerCreateInfo {};
        samplerCreateInfo.minFilter = FilterMode::LINEAR;
        samplerCreateInfo.magFilter = FilterMode::LINEAR;
        noiseSampler = device->CreateSampler(&samplerCreateInfo);

        auto texCommandBuffer = device->CreateCommandBuffer();
        auto* commandEncoder = texCommandBuffer->Begin();
        // Dx need not to transition resource state before copy
        commandEncoder->ResourceBarrier(Barrier::Transition(noise.tex, TextureState::UNDEFINED, TextureState::COPY_DST));
        TextureSubResourceInfo subResourceInfo {};
        subResourceInfo.mipLevel = 0;
        subResourceInfo.arrayLayerNum = 1;
        subResourceInfo.baseArrayLayer = 0;
        subResourceInfo.aspect = TextureAspect::COLOR;
        commandEncoder->CopyBufferToTexture(pixelBuffer, noise.tex, &subResourceInfo, {SSAO_NOISE_DIM, SSAO_NOISE_DIM, 1});
        commandEncoder->ResourceBarrier(Barrier::Transition(noise.tex, TextureState::COPY_DST, TextureState::SHADER_READ_ONLY));
        commandEncoder->End();

        graphicsQueue->Submit(texCommandBuffer, nullptr);

    }

    void Renderer::CreateAttachments(RHI::PixelFormat format, RHI::TextureAspect aspect, ColorAttachment* attachment, uint32_t width, uint32_t height)
    {
        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = format;
        texCreateInfo.mipLevels = 1;
        texCreateInfo.extent = {width, height, 1};
        texCreateInfo.dimension = TextureDimension::T_2D;
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::COPY_DST | TextureUsageBits::TEXTURE_BINDING;
        attachment->texture = device->CreateTexture(&texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::TV_2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = 1;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = 1;
        viewCreateInfo.aspect = TextureAspect::COLOR;
        attachment->view = attachment->texture->CreateTextureView(&viewCreateInfo);
    }

    ShaderModule* Renderer::CompileShader(const std::string& fileName, const std::string& entryPoint, RHI::ShaderStageBits shaderStage)
    {
        std::string shaderSource = Common::FileUtils::ReadTextFile(fileName);

        Render::ShaderCompileInput info;
        info.source = shaderSource;
        info.entryPoint = entryPoint;
        info.stage = shaderStage;
        Render::ShaderCompileOptions options;
        if (rhiType == RHI::RHIType::DIRECTX_12) {
            options.byteCodeType = Render::ShaderByteCodeType::DXIL;
            options.definitions.emplace_back("VULKAN=0");
        } else if (rhiType == RHI::RHIType::VULKAN) {
            options.byteCodeType = Render::ShaderByteCodeType::SPRIV;
            options.definitions.emplace_back("VULKAN=1");
        } else if (rhiType == RHI::RHIType::METAL) {
            options.byteCodeType = Render::ShaderByteCodeType::MBC;
            options.definitions.emplace_back("VULKAN=0");
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

        return device->CreateShaderModule(&createInfo);
    }

    void Renderer::CreateUniformBuffer(RHI::BufferUsageFlags flags, UBuffer* uBuffer, size_t size, void* data)
    {
        BufferCreateInfo createInfo {};
        createInfo.size = size;
        createInfo.usages = flags;

        uBuffer->buf = device->CreateBuffer(&createInfo);
        if (uBuffer->buf != nullptr && data != nullptr) {
            auto* mapData = uBuffer->buf->Map(MapMode::WRITE, 0, size);
            memcpy(mapData, data, size);
            uBuffer->buf->UnMap();
        }

        BufferViewCreateInfo viewCreateInfo {};
        viewCreateInfo.size = size;
        viewCreateInfo.offset = 0;
        uBuffer->bufView = uBuffer->buf->CreateBufferView(&viewCreateInfo);
    }

    void Renderer::CreatePipeline()
    {
        shaderModules.gBufferVert     = CompileShader("shaders/gbuffer.vert", "VSMain", ShaderStageBits::VERTEX);
        shaderModules.gBufferFrag     = CompileShader("shaders/gbuffer.frag", "FSMain", ShaderStageBits::FRAGMENT);
        shaderModules.quadVert        = CompileShader("shaders/fullscreen.vert", "VSMain", ShaderStageBits::VERTEX);
        shaderModules.ssaoFrag        = CompileShader("shaders/ssao.frag", "FSMain", ShaderStageBits::FRAGMENT);
        shaderModules.ssaoBlurFrag    = CompileShader("shaders/blur.frag", "FSMain", ShaderStageBits::FRAGMENT);
        shaderModules.compositionFrag = CompileShader("shaders/composition.frag", "FSMain", ShaderStageBits::FRAGMENT);

        // Gbuffer vertex
        std::array<VertexAttribute, 4> vertexAttributes {};
        vertexAttributes[0].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[0].offset = 0;
        vertexAttributes[0].semanticName = "POSITION0";
        vertexAttributes[0].semanticIndex = 0;
        vertexAttributes[1].format = VertexFormat::FLOAT32_X2;
        vertexAttributes[1].offset = offsetof(Vertex, uv);
        vertexAttributes[1].semanticName = "TEXCOORD0";
        vertexAttributes[1].semanticIndex = 0;
        vertexAttributes[2].format = VertexFormat::FLOAT32_X4;
        vertexAttributes[2].offset = offsetof(Vertex, color);
        vertexAttributes[2].semanticName = "COLOR0";
        vertexAttributes[2].semanticIndex = 0;
        vertexAttributes[3].format = VertexFormat::FLOAT32_X3;
        vertexAttributes[3].offset = offsetof(Vertex, normal);
        vertexAttributes[3].semanticName = "NORMAL0";
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
        quadVertexAttributes[0].semanticName = "POSITION0";
        quadVertexAttributes[0].semanticIndex = 0;
        quadVertexAttributes[1].format = VertexFormat::FLOAT32_X2;
        quadVertexAttributes[1].offset = offsetof(QuadVertex, uv);
        quadVertexAttributes[1].semanticName = "TEXCOORD0";
        quadVertexAttributes[1].semanticIndex = 0;

        VertexBufferLayout quadVertexBufferLayout {};
        quadVertexBufferLayout.stepMode = RHI::VertexStepMode::PER_VERTEX;
        quadVertexBufferLayout.stride = sizeof(QuadVertex);
        quadVertexBufferLayout.attributeNum = quadVertexAttributes.size();
        quadVertexBufferLayout.attributes = quadVertexAttributes.data();

        // General pipeline infos
        GraphicsPipelineCreateInfo createInfo {};
        createInfo.vertex.bufferLayoutNum = 1;
        createInfo.primitive.depthClip = false;
        createInfo.primitive.frontFace = RHI::FrontFace::CCW;
        createInfo.primitive.cullMode = CullMode::NONE;
        createInfo.primitive.topologyType = RHI::PrimitiveTopologyType::TRIANGLE;
        createInfo.primitive.stripIndexFormat = IndexFormat::UINT32;
        createInfo.depthStencil.depthEnable = false;
        createInfo.depthStencil.stencilEnable = false;
        createInfo.multiSample.count = 1;

        // Gbuffer
        std::array<ColorTargetState, 3> colorTargetStates0 {};
        colorTargetStates0[0].format = PixelFormat::RGBA32_FLOAT;
        colorTargetStates0[0].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;
        colorTargetStates0[1].format = PixelFormat::RGBA8_UNORM;
        colorTargetStates0[1].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;
        colorTargetStates0[2].format = PixelFormat::RGBA8_UNORM;
        colorTargetStates0[2].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

        createInfo.vertex.bufferLayouts = &vertexBufferLayout;
        createInfo.fragment.colorTargetNum = colorTargetStates0.size();
        createInfo.fragment.colorTargets = colorTargetStates0.data();
        createInfo.vertexShader = shaderModules.gBufferVert;
        createInfo.fragmentShader = shaderModules.gBufferFrag;
        createInfo.layout = pipelineLayouts.gBuffer;
        pipelines.gBuffer = device->CreateGraphicsPipeline(&createInfo);

        // ssao
        std::array<ColorTargetState, 1> colorTargetStates1 {};
        colorTargetStates1[0].format = PixelFormat::R8_UNORM;
        colorTargetStates1[0].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

        createInfo.vertex.bufferLayouts = &quadVertexBufferLayout;
        createInfo.fragment.colorTargetNum = colorTargetStates1.size();
        createInfo.fragment.colorTargets = colorTargetStates1.data();
        createInfo.vertexShader = shaderModules.quadVert;
        createInfo.fragmentShader = shaderModules.ssaoFrag;
        createInfo.layout = pipelineLayouts.ssao;
        pipelines.ssao = device->CreateGraphicsPipeline(&createInfo);

        // ssaoBlur
        std::array<ColorTargetState, 1> colorTargetStates2 {};
        colorTargetStates2[0].format = PixelFormat::R8_UNORM;
        colorTargetStates2[0].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

        createInfo.vertex.bufferLayouts = &quadVertexBufferLayout;
        createInfo.fragment.colorTargetNum = colorTargetStates2.size();
        createInfo.fragment.colorTargets = colorTargetStates2.data();
        createInfo.vertexShader = shaderModules.quadVert;
        createInfo.fragmentShader = shaderModules.ssaoBlurFrag;
        createInfo.layout = pipelineLayouts.ssaoBlur;
        pipelines.ssaoBlur = device->CreateGraphicsPipeline(&createInfo);

        // composition
        std::array<ColorTargetState, 1> colorTargetStates3 {};
        colorTargetStates3[0].format = PixelFormat::RGBA8_UNORM;
        colorTargetStates3[0].writeFlags = ColorWriteBits::RED | ColorWriteBits::GREEN | ColorWriteBits::BLUE | ColorWriteBits::ALPHA;

        createInfo.vertex.bufferLayouts = &quadVertexBufferLayout;
        createInfo.fragment.colorTargetNum = colorTargetStates3.size();
        createInfo.fragment.colorTargets = colorTargetStates3.data();
        createInfo.vertexShader = shaderModules.quadVert;
        createInfo.fragmentShader = shaderModules.compositionFrag;
        createInfo.layout = pipelineLayouts.composition;
        pipelines.composition = device->CreateGraphicsPipeline(&createInfo);
    }

    void Renderer::PopulateCommandBuffer()
    {
        CommandEncoder* commandEncoder = commandBuffer->Begin();
        {
            // GBuffer
            std::array<GraphicsPassColorAttachment, 3> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = gBufferOutput.pos.view;
            colorAttachments[0].resolve = nullptr;
            colorAttachments[1].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[1].loadOp = LoadOp::CLEAR;
            colorAttachments[1].storeOp = StoreOp::STORE;
            colorAttachments[1].view = gBufferOutput.normal.view;
            colorAttachments[1].resolve = nullptr;
            colorAttachments[2].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[2].loadOp = LoadOp::CLEAR;
            colorAttachments[2].storeOp = StoreOp::STORE;
            colorAttachments[2].view = gBufferOutput.albedo.view;
            colorAttachments[2].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.pipeline = pipelines.gBuffer;
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.scene);
                graphicsEncoder->SetVertexBuffer(0, vertexBufferView);
                graphicsEncoder->SetIndexBuffer(indexBufferView);

                for (auto renderable : renderables) {
                    graphicsEncoder->SetBindGroup(1, renderable.bindGroup);
                    graphicsEncoder->DrawIndexed(renderable.indexCount, 1, renderable.firstIndex, 0, 0);
                }
            }
            graphicsEncoder->EndPass();

            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferOutput.pos.texture, TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferOutput.normal.texture, TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
            commandEncoder->ResourceBarrier(Barrier::Transition(gBufferOutput.albedo.texture, TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
        }

        {
            // ssao
            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = ssaoOutput.view;
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.pipeline = pipelines.ssao;
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.composition);
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView);
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView);
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoOutput.texture, TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
        }

        {
            // ssaoBlur
            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = ssaoBlurOutput.view;
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.pipeline = pipelines.ssaoBlur;
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.composition);
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView);
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView);
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(ssaoBlurOutput.texture, TextureState::RENDER_TARGET, TextureState::SHADER_READ_ONLY));
        }

        {
            auto backTextureIndex = swapChain->AcquireBackTexture();

            // composition
            std::array<GraphicsPassColorAttachment, 1> colorAttachments {};
            colorAttachments[0].clearValue = ColorNormalized<4> {0.0f, 0.0f, 0.0f, 1.0f};
            colorAttachments[0].loadOp = LoadOp::CLEAR;
            colorAttachments[0].storeOp = StoreOp::STORE;
            colorAttachments[0].view = swapChainTextureViews[backTextureIndex];
            colorAttachments[0].resolve = nullptr;

            GraphicsPassBeginInfo graphicsPassBeginInfo {};
            graphicsPassBeginInfo.pipeline = pipelines.composition;
            graphicsPassBeginInfo.colorAttachmentNum = colorAttachments.size();
            graphicsPassBeginInfo.colorAttachments = colorAttachments.data();
            graphicsPassBeginInfo.depthStencilAttachment = nullptr;

            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::PRESENT, TextureState::RENDER_TARGET));
            auto* graphicsEncoder = commandEncoder->BeginGraphicsPass(&graphicsPassBeginInfo);
            {
                graphicsEncoder->SetScissor(0, 0, app->width, app->height);
                graphicsEncoder->SetViewport(0, 0, app->width, app->height, 0, 1);
                graphicsEncoder->SetPrimitiveTopology(PrimitiveTopology::TRIANGLE_LIST);
                graphicsEncoder->SetBindGroup(0, bindGroups.composition);
                graphicsEncoder->SetVertexBuffer(0, quadVertexBufferView);
                graphicsEncoder->SetIndexBuffer(quadIndexBufferView);
                graphicsEncoder->DrawIndexed(6, 1, 0, 0, 0);
            }
            graphicsEncoder->EndPass();
            commandEncoder->ResourceBarrier(Barrier::Transition(swapChainTextures[backTextureIndex], TextureState::RENDER_TARGET, TextureState::PRESENT));
        }

        commandEncoder->SwapChainSync(swapChain);
        commandEncoder->End();
    }

    void Renderer::SubmitCommandBufferAndPresent()
    {
        graphicsQueue->Submit(commandBuffer, fence);
        fence->Wait();
        swapChain->Present();
    }

    void Renderer::InitCamera()
    {
        camera.type = Camera::CameraType::firstPerson;
        camera.position = { 1.0f, 0.75f, 0.0f };
        camera.setRotation(glm::vec3(0.0f, 90.0f, 0.0f));
        camera.setPerspective(60.0f, (float)app->width / (float)app->height, uboSceneParams.nearPlane, uboSceneParams.farPlane);
    }

    void Renderer::LoadGLTF()
    {
        model = new Model();
        model->LoadFromFile("models/voyager.gltf");
    }

    Renderable::~Renderable()
    {
        diffuseColorMap->Destroy();
        diffuseColorMapView->Destroy();
        bindGroup->Destroy();
    }

    void Renderer::GenerateRenderables()
    {
        for (auto* node : model->nodes) {
            for (auto* primitive : node->mesh->primitives) {
                Renderable renderable;
                renderable.InitalizeWithPrimitive(this, primitive);

                renderables.emplace_back(std::move(renderable));
            }
        }
    }

    void Renderable::InitalizeWithPrimitive(Renderer* renderer ,Primitive* primitive)
    {
        // upload diffuseColorMap
        auto* data = primitive->materialData->baseColorTexture;

        BufferCreateInfo bufferCreateInfo {};
        bufferCreateInfo.size = data->size;
        bufferCreateInfo.usages = BufferUsageBits::UNIFORM | BufferUsageBits::MAP_WRITE | BufferUsageBits::COPY_SRC;
        auto* pixelBuffer = renderer->GetDevice()->CreateBuffer(&bufferCreateInfo);
        if (pixelBuffer != nullptr) {
            auto* mapData = pixelBuffer->Map(MapMode::WRITE, 0, bufferCreateInfo.size);
            memcpy(mapData, data->pixels, bufferCreateInfo.size);
            pixelBuffer->UnMap();
        }

        TextureCreateInfo texCreateInfo {};
        texCreateInfo.format = PixelFormat::RGBA8_UNORM;
        texCreateInfo.mipLevels = data->mipLevels;
        texCreateInfo.extent = { data->width, data->height, 1};
        texCreateInfo.samples = 1;
        texCreateInfo.usages = TextureUsageBits::COPY_DST | TextureUsageBits::TEXTURE_BINDING;
        diffuseColorMap = renderer->GetDevice()->CreateTexture(&texCreateInfo);

        TextureViewCreateInfo viewCreateInfo {};
        viewCreateInfo.dimension = TextureViewDimension::TV_2D;
        viewCreateInfo.baseArrayLayer = 0;
        viewCreateInfo.arrayLayerNum = data->arrayLayers;
        viewCreateInfo.baseMipLevel = 0;
        viewCreateInfo.mipLevelNum = data->mipLevels;
        viewCreateInfo.aspect = TextureAspect::COLOR;
        diffuseColorMapView = diffuseColorMap->CreateTextureView(&viewCreateInfo);

        auto texCommandBuffer = renderer->GetDevice()->CreateCommandBuffer();
        auto* commandEncoder = texCommandBuffer->Begin();
        // Dx need not to transition resource state before copy
        commandEncoder->ResourceBarrier(Barrier::Transition(diffuseColorMap, TextureState::UNDEFINED, TextureState::COPY_DST));
        TextureSubResourceInfo subResourceInfo {};
        subResourceInfo.mipLevel = 0;
        subResourceInfo.arrayLayerNum = 1;
        subResourceInfo.baseArrayLayer = 0;
        subResourceInfo.aspect = TextureAspect::COLOR;
        commandEncoder->CopyBufferToTexture(pixelBuffer, diffuseColorMap, &subResourceInfo, {data->width, data->height, 1});
        commandEncoder->ResourceBarrier(Barrier::Transition(diffuseColorMap, TextureState::COPY_DST, TextureState::SHADER_READ_ONLY));
        commandEncoder->End();

        renderer->GetQueue()->Submit(texCommandBuffer, nullptr);

        // per renderable bindGroup
        std::vector<BindGroupEntry> entries(2);
        entries[0].type = BindingType::TEXTURE;
        entries[0].binding = 0;
        entries[0].textureView = diffuseColorMapView;
        entries[1].type = BindingType::SAMPLER;
        entries[1].binding = 1;
        entries[1].sampler = renderer->GetSampler();

        BindGroupCreateInfo createInfo {};
        createInfo.entries = entries.data();
        createInfo.entryNum = static_cast<uint32_t>(entries.size());
        createInfo.layout = renderer->GetLayout();

        bindGroup = renderer->GetDevice()->CreateBindGroup(&createInfo);
    }
}
