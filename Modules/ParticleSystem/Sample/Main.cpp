//
// Created by John Kindem on 2021/4/4 0004.
//

#include <Application/Application.h>

#include <Explosion/RHI/Vulkan/VulkanDriver.h>
#include <Explosion/Common/FileReader.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_MAC
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#ifdef _WIN32
#define GLFW_EXPOSE_NATIVE_WIN32
#endif
#include <GLFW/glfw3native.h>

#include <ParticleSystem.h>
#include <Emitters/ParticleSpriteEmitter.h>
#include <Effectors/ParticleLocationEffector.h>
#include <Effectors/ParticleLifeTimeEffector.h>
#include <Effectors/ParticleForceEffector.h>
#include <Effectors/ParticleCollisionEffector.h>
#include <Effectors/ParticleVelocityEffector.h>

#include <Explosion/Common/Math.h>

#include <chrono>
#include <iostream>
#include <thread>
#include <Effectors/ParticleForceEffector.h>

using namespace Explosion;
using namespace Explosion::RHI;

struct Ubo {
    Math::Matrix<4> view;
    Math::Matrix<4> proj;
};

class App : public Application {
public:
    App(const std::string& name, uint32_t width, uint32_t height) : Application(name, width, height) {}

protected:
    void OnStart() override
    {
        driver = std::make_unique<VulkanDriver>();

        SwapChain::Config swapChainConfig {};
        swapChainConfig.width = GetWidth();
        swapChainConfig.height = GetHeight();
#ifdef TARGET_OS_MAC
        swapChainConfig.surface = static_cast<void*>(glfwGetCocoaWindow(GetWindow()));
#endif
#ifdef _WIN32
        swapChainConfig.surface = static_cast<void*>(glfwGetWin32Window(GetWindow()));
#endif
        swapChain = driver->CreateSwapChain(swapChainConfig);

        ImageView::Config imageViewConfig {};
        imageViewConfig.type = ImageViewType::VIEW_2D;
        imageViewConfig.mipLevelCount = 1;
        imageViewConfig.baseMipLevel = 0;
        imageViewConfig.layerCount = 1;
        imageViewConfig.baseLayer = 0;
        imageViewConfig.aspects = FlagsCast(ImageAspectBits::COLOR);
        imageViews.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < imageViews.size(); i++) {
            imageViewConfig.image = swapChain->GetColorAttachments()[i];
            imageViews[i] = driver->CreateImageView(imageViewConfig);
        }
        
        RenderPass::Config renderPassConfig {};
        renderPassConfig.attachmentConfigs = {{
            AttachmentType::SWAP_CHAIN_COLOR_ATTACHMENT,
            swapChain->GetSurfaceFormat(),
            AttachmentLoadOp::CLEAR,
            AttachmentStoreOp::STORE
        }};
        renderPass = driver->CreateRenderPass(renderPassConfig);

        FrameBuffer::Config frameBufferConfig {};
        frameBufferConfig.renderPass = renderPass;
        frameBufferConfig.width = GetWidth();
        frameBufferConfig.height = GetHeight();
        frameBufferConfig.layers = 1;
        frameBuffers.resize(swapChain->GetColorAttachmentCount());
        for (uint32_t i = 0; i < frameBuffers.size(); i++) {
            frameBufferConfig.attachments = { imageViews[i] };
            frameBuffers[i] = driver->CreateFrameBuffer(frameBufferConfig);
        }

        GraphicsPipeline::Config pipelineConfig {};
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.shaderConfig.shaderModules = {
            { ShaderStageBits::VERTEX,   FileReader::Read("ParticleSpriteVertex.spv") },
            { ShaderStageBits::FRAGMENT, FileReader::Read("ParticleSpriteFragment.spv") }
        };

        pipelineConfig.vertexConfig.vertexBindings = {
            { 0, sizeof(Particle), VertexInputRate::PER_VERTEX }
        };

        pipelineConfig.vertexConfig.vertexAttributes = {
            { 0, 0, Format::R32_G32_B32_FLOAT, static_cast<uint32_t>(offsetof(Particle, position)) },
        };

        pipelineConfig.descriptorConfig.descriptorAttributes = {
            {0, DescriptorType::UNIFORM_BUFFER, (uint32_t)ShaderStageBits::VERTEX},
        };

        pipelineConfig.assemblyConfig.topology = PrimitiveTopology::POINT_LIST;

        pipelineConfig.viewportScissorConfig.viewport = { 0, 0, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()), 0, 1.0 };
        pipelineConfig.viewportScissorConfig.scissor = { 0, 0, GetWidth(), GetHeight() };
        pipelineConfig.rasterizerConfig = { false, false, FlagsCast(CullModeBits::NONE), FrontFace::CLOCK_WISE };
        pipelineConfig.depthStencilConfig = { false, false, false };
        pipelineConfig.colorBlendConfig.enabled = false;
        pipeline = driver->CreateGraphicsPipeline(pipelineConfig);

        Buffer::Config bufferConfig {};
        bufferConfig.size = sizeof(Particle) * 1000;
        bufferConfig.usages = BufferUsageBits::VERTEX_BUFFER | BufferUsageBits::TRANSFER_DST;
        bufferConfig.memoryProperties = FlagsCast(MemoryPropertyBits::HOST_VISIBLE) | FlagsCast(MemoryPropertyBits::HOST_COHERENT);
        vertexBuffer = driver->CreateBuffer(bufferConfig);

        Buffer::Config uboConfig {};
        uboConfig.size = sizeof(Ubo);
        uboConfig.usages = BufferUsageBits::UNIFORM_BUFFER | BufferUsageBits::TRANSFER_DST;
        uboConfig.memoryProperties = FlagsCast(MemoryPropertyBits::HOST_VISIBLE) | FlagsCast(MemoryPropertyBits::HOST_COHERENT);
        uniformBuffer = driver->CreateBuffer(uboConfig);

        DescriptorPool::Config poolConfig = {};
        poolConfig.maxSets = 1000;
        poolConfig.poolSizes = {
                {DescriptorType::UNIFORM_BUFFER, 1000},
        };
        pool = driver->CreateDescriptorPool(poolConfig);
        set = driver->AllocateDescriptorSet(pool, pipeline);

        DescriptorSet::DescriptorBufferInfo dbi = {};
        dbi.buffer = uniformBuffer;
        dbi.offset = 0;
        dbi.range = sizeof(Ubo);
        set->WriteDescriptors({
            { 0, DescriptorType::UNIFORM_BUFFER, &dbi, nullptr}
        });

        emitter = particleSystem.CreateEmitter<ParticleSpriteEmitter>(ParticleSpriteEmitter::Descriptor{});
        box.SetRandomDevice(random);
        cone.SetRandomDevice(random);
        ground.SetGroundHeight(-1.f);

        emitter->AddEffector(&lifetime, PARTICLE_EFFECT_BOTH);
        emitter->AddEffector(&point, PARTICLE_EFFECT_SPAWN);
        emitter->AddEffector(&gravity, PARTICLE_EFFECT_UPDATE);
        emitter->AddEffector(&cone, PARTICLE_EFFECT_SPAWN);
//        emitter->AddEffector(&ground, PARTICLE_EFFECT_UPDATE);
    }

    void OnStop() override
    {
        driver->DestroyBuffer(vertexBuffer);
        driver->DestroyBuffer(uniformBuffer);
        driver->DestroyDescriptorPool(pool);

        driver->DestroyGraphicsPipeline(pipeline);
        for (auto* frameBuffer : frameBuffers) {
            driver->DestroyFrameBuffer(frameBuffer);
        }
        driver->DestroyRenderPass(renderPass);
        for (auto* imageView : imageViews) {
            driver->DestroyImageView(imageView);
        }
        driver->DestroySwapChain(swapChain);
    }

    void OnDrawFrame() override
    {
        static auto start = std::chrono::steady_clock::now();
        auto curr = std::chrono::steady_clock::now();
        auto delta = std::chrono::duration<float, std::milli>(curr - start).count();

        particleSystem.Tick(delta / 1000.f);

        std::this_thread::sleep_for(std::chrono::milliseconds (16));
        start = curr;

        swapChain->DoFrame([this](uint32_t imageIdx, Signal* imageReadySignal, Signal* frameFinishedSignal) -> void {
            auto* commandBuffer = driver->CreateCommandBuffer();
            commandBuffer->EncodeCommands([imageIdx, this](CommandEncoder* encoder) -> void {

                CommandEncoder::RenderPassBeginInfo beginInfo {};
                beginInfo.frameBuffer = frameBuffers[imageIdx];
                beginInfo.renderArea = { 0, 0, GetWidth(), GetHeight() };
                beginInfo.clearValue = { 0.f, 0.f, 0.f, 1.f };
                encoder->BeginRenderPass(renderPass, beginInfo);

                void* vtx = const_cast<uint8_t*>(emitter->GetVertexData());
                if (vtx != nullptr) {
                    vertexBuffer->UpdateData(vtx);
                    ubo.view = glm::inverse(
                            glm::lookAt(glm::vec3(0.f, 0.f, -5.f), glm::vec3(0.f, 0.f, 0.f), glm::vec3(0.f, 1.f, 0.f)));
                    ubo.proj = glm::perspective(60.f / 180.f * 3.14f, GetWidth() / (float) GetHeight(), 0.01f, 100.f);
                    uniformBuffer->UpdateData((void *) &ubo);

                    encoder->BindGraphicsPipeline(pipeline);
                    encoder->SetViewPort(
                            {0, 0, static_cast<float>(GetWidth()), static_cast<float>(GetHeight()), 0.f, 1.f});
                    encoder->SetScissor({0, 0, GetWidth(), GetHeight()});
                    encoder->BindDescriptorSet(set, 1);
                    encoder->BindVertexBuffer(0, vertexBuffer);
                    encoder->Draw(0, emitter->GetActiveCount(), 0, 1);
                }
                encoder->EndRenderPass();
            });
            commandBuffer->Submit(imageReadySignal, frameFinishedSignal, FlagsCast(PipelineStageBits::COLOR_ATTACHMENT_OUTPUT));
        });
    }

private:
    std::unique_ptr<Driver> driver;
    SwapChain* swapChain = nullptr;
    std::vector<ImageView*> imageViews;
    RenderPass* renderPass = nullptr;
    std::vector<FrameBuffer*> frameBuffers;
    GraphicsPipeline* pipeline = nullptr;
    DescriptorPool* pool = nullptr;
    DescriptorSet* set = nullptr;
    Buffer* uniformBuffer = nullptr;
    Buffer* vertexBuffer = nullptr;

    ParticleSystem particleSystem;
    ParticleBoxLocation box;
    ParticlePointLocation point;
    ParticleLifeTimeEffector lifetime;
    ParticleGravityEffector gravity;
    ParticleGroundCollision ground;
    ParticleConeEffector cone;
    ParticleSpriteEmitter* emitter;
    RandomDevice random;

    Ubo ubo;
};

int main(int argc, char* argv[])
{
    App app("Triangle", 1024, 768);
    app.Run();
    return 0;
}
