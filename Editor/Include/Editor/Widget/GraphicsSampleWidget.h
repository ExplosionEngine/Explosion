//
// Created by Kindem on 2025/3/16.
//

#pragma once

#include <QWidget>
#include <QBoxLayout>

#include <Render/ShaderCompiler.h>
#include <Editor/Widget/GraphicsWidget.h>

namespace Editor {
    class GraphicsSampleWidget final : public GraphicsWidget {
        Q_OBJECT

    public:
        explicit GraphicsSampleWidget(QWidget* inParent = nullptr);
        ~GraphicsSampleWidget() override;

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:
        static constexpr uint8_t swapChainTextureNum = 2;

        void RecreateSwapChain(uint32_t inWidth, uint32_t inHeight);
        void DispatchFrame() const;
        void DrawFrame() const;

        Common::UniquePtr<RHI::Semaphore> imageReadySemaphore;
        Common::UniquePtr<RHI::Semaphore> renderFinishedSemaphore;
        Common::UniquePtr<RHI::Fence> frameFence;
        Common::UniquePtr<RHI::SwapChain> swapChain;
        std::array<RHI::Texture*, 2> swapChainTextures;
        std::array<Common::UniquePtr<RHI::TextureView>, 2> swapChainTextureViews;
        Render::ShaderCompileOutput vsCompileOutput;
        Render::ShaderCompileOutput psCompileOutput;
        Common::UniquePtr<RHI::ShaderModule> vsModule;
        Common::UniquePtr<RHI::ShaderModule> psModule;
        Common::UniquePtr<RHI::BindGroupLayout> bindGroupLayout;
        Common::UniquePtr<RHI::PipelineLayout> pipelineLayout;
        Common::UniquePtr<RHI::RasterPipeline> pipeline;
        Common::UniquePtr<RHI::Buffer> vertexBuffer;
        Common::UniquePtr<RHI::BufferView> vertexBufferView;
        Common::UniquePtr<RHI::Buffer> uniformBuffer;
        Common::UniquePtr<RHI::BufferView> uniformBufferView;
        Common::UniquePtr<RHI::BindGroup> bindGroup;
        Common::UniquePtr<RHI::CommandBuffer> commandBuffer;
        Common::UniquePtr<Common::WorkerThread> drawThread;
        std::atomic_bool running;
    };
}
