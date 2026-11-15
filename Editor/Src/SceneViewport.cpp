#include <GLFW/glfw3.h>

#include <Editor/SceneViewport.h>
#include <Editor/System/Camera.h>
#include <Runtime/Component/Camera.h>
#include <Runtime/Component/Transform.h>
#include <Runtime/World.h>

namespace Editor::SceneViewing::Internal {
    static Render::CanvasDesc SceneCanvasDesc()
    {
        Render::CanvasDesc desc;
        desc.width = 1280;
        desc.height = 720;
        desc.debugName = "editorSceneColor";
        return desc;
    }
}

namespace Editor {
    SceneViewport::SceneViewport(Runtime::World& inWorld, GLFWwindow* inWindow, RHI::Device& inDevice)
        : world(inWorld)
        , window(inWindow)
        , client(world)
        , canvas(inDevice, SceneViewing::Internal::SceneCanvasDesc())
        , camera(CreateCamera())
        , visible(false)
        , hovered(false)
    {
    }

    SceneViewport::~SceneViewport()
    {
        EndCameraLook();
        WaitIdle();
        world.EditorAccess([this](Runtime::ECRegistry& inRegistry) -> void {
            inRegistry.Destroy(camera);
        });
    }

    void SceneViewport::Render(float inDeltaSeconds)
    {
        if (visible) {
            Runtime::Viewport::Render(inDeltaSeconds);
        }
    }

    void SceneViewport::Resize(uint32_t inWidth, uint32_t inHeight)
    {
        if (canvas.GetSize() == Common::UVec2(inWidth, inHeight)) {
            return;
        }
        WaitIdle();
        canvas.Resize(inWidth, inHeight);
    }

    RHI::TextureView* SceneViewport::GetTextureView() const
    {
        return canvas.GetTextureView();
    }

    void SceneViewport::SetVisible(bool inVisible)
    {
        visible = inVisible;
        if (!visible) {
            EndCameraLook();
            hovered = false;
            SetFocused(false);
        }
    }

    void SceneViewport::SetHovered(bool inHovered)
    {
        hovered = inHovered;
    }

    void SceneViewport::SetFocused(bool inFocused)
    {
        if (!inFocused && !IsCameraLooking()) {
            world.EditorAccess([this](Runtime::ECRegistry& inRegistry) -> void {
                inRegistry.Update<EditorCameraInput>(camera, [](EditorCameraInput& input) -> void {
                    input.moveForward = false;
                    input.moveBackward = false;
                    input.moveLeft = false;
                    input.moveRight = false;
                });
            });
        }
    }

    bool SceneViewport::IsHovered() const
    {
        return hovered;
    }

    bool SceneViewport::IsCameraLooking() const
    {
        bool looking = false;
        world.EditorAccess([&](Runtime::ECRegistry& inRegistry) -> void {
            looking = inRegistry.Get<EditorCameraInput>(camera).looking;
        });
        return looking;
    }

    void SceneViewport::OnKey(int inKey, bool inPressed)
    {
        if (inKey != GLFW_KEY_W && inKey != GLFW_KEY_S && inKey != GLFW_KEY_A && inKey != GLFW_KEY_D) {
            return;
        }
        world.EditorAccess([&](Runtime::ECRegistry& inRegistry) -> void {
            inRegistry.Update<EditorCameraInput>(camera, [&](EditorCameraInput& input) -> void {
                if (inKey == GLFW_KEY_W) { input.moveForward = inPressed; }
                if (inKey == GLFW_KEY_S) { input.moveBackward = inPressed; }
                if (inKey == GLFW_KEY_A) { input.moveLeft = inPressed; }
                if (inKey == GLFW_KEY_D) { input.moveRight = inPressed; }
            });
        });
    }

    void SceneViewport::BeginCameraLook()
    {
        if (!hovered || IsCameraLooking()) {
            return;
        }
        world.EditorAccess([this](Runtime::ECRegistry& inRegistry) -> void {
            inRegistry.Update<EditorCameraInput>(camera, [](EditorCameraInput& input) -> void {
                input.looking = true;
                input.lookDelta = Common::FVec2Consts::zero;
            });
        });
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    }

    void SceneViewport::EndCameraLook()
    {
        if (!IsCameraLooking()) {
            return;
        }
        world.EditorAccess([this](Runtime::ECRegistry& inRegistry) -> void {
            inRegistry.Update<EditorCameraInput>(camera, [](EditorCameraInput& input) -> void {
                input.moveForward = false;
                input.moveBackward = false;
                input.moveLeft = false;
                input.moveRight = false;
                input.looking = false;
                input.lookDelta = Common::FVec2Consts::zero;
            });
        });
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }

    void SceneViewport::AddCameraLookDelta(float inDeltaX, float inDeltaY)
    {
        world.EditorAccess([&](Runtime::ECRegistry& inRegistry) -> void {
            inRegistry.Update<EditorCameraInput>(camera, [&](EditorCameraInput& input) -> void {
                input.lookDelta += Common::FVec2(inDeltaX, inDeltaY);
            });
        });
    }

    Runtime::Entity SceneViewport::CreateCamera()
    {
        Runtime::Entity entity = Runtime::entityNull;
        world.EditorAccess([&](Runtime::ECRegistry& inRegistry) -> void {
            entity = inRegistry.Create();
            inRegistry.AddTag<Runtime::TransientTag>(entity);
            inRegistry.Emplace<Runtime::Camera>(entity);
            inRegistry.Emplace<EditorCameraController>(entity);
            inRegistry.Emplace<EditorCameraInput>(entity);
            inRegistry.Emplace<Runtime::WorldTransform>(entity, Common::FTransform::LookAt(
                Common::FVec3(-5.0f, -6.0f, 4.0f), Common::FVec3(0.0f, 0.0f, 0.5f)));
        });
        return entity;
    }

    Runtime::Client& SceneViewport::GetClient()
    {
        return client;
    }

    Render::RenderSurface& SceneViewport::GetRenderSurface()
    {
        return canvas;
    }

    std::vector<Render::View> SceneViewport::BuildViews(Runtime::ECRegistry& inRegistry, Common::UVec2 inSize)
    {
        return { BuildViewForCamera(inRegistry, camera, Common::URect::FromMinExtent(0, 0, inSize.x, inSize.y)) };
    }
}
