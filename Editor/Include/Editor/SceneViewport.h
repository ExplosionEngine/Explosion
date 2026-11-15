#pragma once

#include <Editor/SceneClient.h>
#include <Render/Canvas.h>
#include <Runtime/Viewport.h>

struct GLFWwindow;

namespace Editor {
    class SceneViewport final : public Runtime::Viewport {
    public:
        SceneViewport(Runtime::World& inWorld, GLFWwindow* inWindow, RHI::Device& inDevice);
        ~SceneViewport() override;

        NonCopyable(SceneViewport)
        NonMovable(SceneViewport)

        void Render(float inDeltaSeconds) override;
        void Resize(uint32_t inWidth, uint32_t inHeight);
        RHI::TextureView* GetTextureView() const;
        void SetVisible(bool inVisible);
        void SetHovered(bool inHovered);
        void SetFocused(bool inFocused);
        bool IsHovered() const;
        bool IsCameraLooking() const;
        void OnKey(int inKey, bool inPressed);
        void BeginCameraLook();
        void EndCameraLook();
        void AddCameraLookDelta(float inDeltaX, float inDeltaY);

    private:
        Runtime::Client& GetClient() override;
        Render::RenderSurface& GetRenderSurface() override;
        std::vector<Render::View> BuildViews(Runtime::ECRegistry& inRegistry, Common::UVec2 inSize) override;
        Runtime::Entity CreateCamera();

        Runtime::World& world;
        GLFWwindow* window;
        SceneClient client;
        Render::Canvas canvas;
        Runtime::Entity camera;
        bool visible;
        bool hovered;
    };
}
