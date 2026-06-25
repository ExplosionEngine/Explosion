//
// Created by johnk on 2026/6/21.
//

#pragma once

#include <array>
#include <atomic>

#include <QWidget>

#include <Render/ShaderCompiler.h>
#include <Editor/Widget/GraphicsWidget.h>
#include <Editor/Widget/WebWidget.h>

namespace Editor {
    class PrototypeTriangleWidget final : public GraphicsWidget {
        Q_OBJECT

    public:
        explicit PrototypeTriangleWidget(QWidget* inParent = nullptr);
        ~PrototypeTriangleWidget() override;

        void SetRotationSpeed(float inRadiansPerSecond);
        float GetRotationSpeed() const;

    protected:
        void resizeEvent(QResizeEvent* event) override;

    private:
        static constexpr uint8_t swapChainTextureNum = 2;

        void RecreateSwapChain(uint32_t inWidth, uint32_t inHeight);
        void DispatchFrame();
        void DrawFrame();

        std::atomic<float> rotationSpeed;
        float rotation;
        double lastFrameSeconds;

        Common::UniquePtr<RHI::Semaphore> imageReadySemaphore;
        Common::UniquePtr<RHI::Semaphore> renderFinishedSemaphore;
        Common::UniquePtr<RHI::Fence> frameFence;
        Common::UniquePtr<RHI::SwapChain> swapChain;
        std::array<RHI::Texture*, swapChainTextureNum> swapChainTextures;
        std::array<Common::UniquePtr<RHI::TextureView>, swapChainTextureNum> swapChainTextureViews;
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

    class PrototypeBackend final : public QObject {
        Q_OBJECT
        Q_PROPERTY(double rotationSpeed READ GetRotationSpeed CONSTANT)

    public:
        explicit PrototypeBackend(PrototypeTriangleWidget* inTriangle, QObject* inParent = nullptr);
        ~PrototypeBackend() override;

    public Q_SLOTS:
        void SetRotationSpeed(double inDegreesPerSecond);

    private:
        double GetRotationSpeed() const;

        PrototypeTriangleWidget* triangle;
    };

    class PrototypeWebWidget final : public WebWidget {
        Q_OBJECT

    public:
        explicit PrototypeWebWidget(PrototypeTriangleWidget* inTriangle, QWidget* inParent = nullptr);
        ~PrototypeWebWidget() override;

    private:
        PrototypeBackend* backend;
    };

    class PrototypePlayground final : public QWidget {
        Q_OBJECT

    public:
        explicit PrototypePlayground(QWidget* inParent = nullptr);
        ~PrototypePlayground() override;

    private:
        PrototypeTriangleWidget* triangle;
        PrototypeWebWidget* web;
    };
}
