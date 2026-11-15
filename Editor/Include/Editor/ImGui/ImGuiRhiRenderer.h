#pragma once

#include <unordered_map>

#include <Common/Math/Vector.h>
#include <Common/Memory.h>
#include <Render/RenderThread.h>
#include <Render/ShaderCompiler.h>
#include <Render/RenderSurface.h>
#include <RHI/RHI.h>

struct ImDrawData;

namespace Common::Internal {
    struct ImGuiPassParams {
        Common::FVec2 displayPos;
        Common::FVec2 displaySize;
    };
}

namespace Editor {
    class ImGuiDrawData;

    class ImGuiRhiRenderer final {
    public:
        ImGuiRhiRenderer(RHI::Device& inDevice, Render::RenderSurface& inSurface, Render::RenderThread& inRenderThread);
        ~ImGuiRhiRenderer();

        NonCopyable(ImGuiRhiRenderer)
        NonMovable(ImGuiRhiRenderer)

        void NewFrame();
        void Submit(const ImDrawData& inDrawData);
        void WaitIdle() const;
        void WaitForPreviousFrame() const;

    private:
        void RenderFrame(const Common::SharedPtr<ImGuiDrawData>& inDrawData);
        void RenderUiPass(ImDrawData& inDrawData, const Render::SurfaceTarget& inTarget);
        void RenderImGuiDrawData(ImDrawData& inDrawData, RHI::RasterPassCommandRecorder& inRecorder);
        void CreateImGuiDeviceObjects();
        void CreateImGuiFontTexture();
        void CreateImGuiDynamicBuffers(int inVertexCount, int inIndexCount);
        void UpdateImGuiBuffers(ImDrawData& inDrawData);
        void UpdateImGuiPassParams(const ImDrawData& inDrawData);
        RHI::BindGroup* GetOrCreateImGuiTextureBindGroup(RHI::TextureView& inTextureView);

        RHI::Device& device;
        Render::RenderSurface& surface;
        Render::RenderThread& renderThread;
        RHI::PixelFormat pipelineFormat;
        Common::SharedPtr<ImGuiDrawData> inFlightDrawData;
        Common::UniquePtr<RHI::Fence> uiFrameFence;
        Common::UniquePtr<RHI::CommandBuffer> uiCommandBuffer;
        Render::ShaderCompileOutput imguiVertexShaderCompileOutput;
        Render::ShaderCompileOutput imguiPixelShaderCompileOutput;
        Common::UniquePtr<RHI::ShaderModule> imguiVertexShader;
        Common::UniquePtr<RHI::ShaderModule> imguiPixelShader;
        Common::UniquePtr<RHI::BindGroupLayout> imguiBindGroupLayout;
        Common::UniquePtr<RHI::PipelineLayout> imguiPipelineLayout;
        Common::UniquePtr<RHI::RasterPipeline> imguiPipeline;
        Common::UniquePtr<RHI::Texture> imguiFontTexture;
        Common::UniquePtr<RHI::TextureView> imguiFontTextureView;
        Common::UniquePtr<RHI::Sampler> imguiImageSampler;
        Common::UniquePtr<RHI::Buffer> imguiPassParamsBuffer;
        Common::UniquePtr<RHI::BufferView> imguiPassParamsBufferView;
        std::unordered_map<RHI::TextureView*, Common::UniquePtr<RHI::BindGroup>> imguiFrameTextureBindGroups;
        Common::UniquePtr<RHI::Buffer> imguiVertexBuffer;
        Common::UniquePtr<RHI::BufferView> imguiVertexBufferView;
        Common::UniquePtr<RHI::Buffer> imguiIndexBuffer;
        Common::UniquePtr<RHI::BufferView> imguiIndexBufferView;
        int imguiVertexBufferCapacity;
        int imguiIndexBufferCapacity;
    };
}
