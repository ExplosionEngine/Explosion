//
// Created by johnk on 2026/7/4.
//

#include <array>
#include <string>

#include <GLFW/glfw3.h>
#include <Editor/EditorWindow.h>
#include <Editor/SceneClient.h>
#include <Editor/System/Camera.h>
#include <Editor/SystemGraphPresets.h>
#include <Runtime/Asset/Asset.h>
#include <Runtime/Asset/Level.h>
#include <Runtime/Asset/Material.h>
#include <Runtime/Asset/Mesh.h>
#include <Runtime/Component/Camera.h>
#include <Runtime/Component/Name.h>
#include <Runtime/Component/Player.h>
#include <Runtime/Component/Primitive.h>
#include <Runtime/Component/Transform.h>

namespace Editor::Internal {
    const Core::Uri mainLevelUri("asset://Game/Maps/Main");
    const Core::Uri defaultUnlitMaterialUri("asset://Game/Materials/DefaultUnlit");
    const Core::Uri lightGrayUnlitMaterialInstanceUri("asset://Game/Materials/LightGrayUnlit");
    const Core::Uri yellowUnlitMaterialInstanceUri("asset://Game/Materials/YellowUnlit");
    const Core::Uri cubeMeshUri("asset://Game/Meshes/Cube");
    static bool AssetExists(const Core::Uri& inUri)
    {
        return Core::AssetUriParser(inUri).Parse().Exists();
    }

    template <typename A>
    static void SaveAsset(const Runtime::AssetPtr<A>& inAsset)
    {
        if (const Common::Path parentDir = Core::AssetUriParser(inAsset->Uri()).Parse().Parent();
            !parentDir.Exists()) {
            parentDir.MakeDir();
        }
        Runtime::AssetManager::Get().Save(inAsset);
    }

    static Runtime::StaticMeshVertices BuildCubeVertices()
    {
        // 24 vertices (4 per face) so each face gets its own uv space, z-up unit cube centered at origin
        static constexpr float h = 0.5f;
        static const std::array<std::array<Common::FVec3, 4>, 6> facePositions = {{
            {{ { -h, -h, h }, { h, -h, h }, { h, h, h }, { -h, h, h } }},     // +z
            {{ { -h, h, -h }, { h, h, -h }, { h, -h, -h }, { -h, -h, -h } }}, // -z
            {{ { h, -h, -h }, { h, h, -h }, { h, h, h }, { h, -h, h } }},     // +x
            {{ { -h, h, -h }, { -h, -h, -h }, { -h, -h, h }, { -h, h, h } }}, // -x
            {{ { h, h, -h }, { -h, h, -h }, { -h, h, h }, { h, h, h } }},     // +y
            {{ { -h, -h, -h }, { h, -h, -h }, { h, -h, h }, { -h, -h, h } }} // -y
        }};

        Runtime::StaticMeshVertices result;
        for (const auto& face : facePositions) {
            const auto baseVertex = static_cast<uint32_t>(result.positions.size());
            for (size_t corner = 0; corner < 4; corner++) {
                result.positions.emplace_back(face[corner]);
                result.tangents.emplace_back(0.0f, 0.0f, 1.0f);
                result.uv0.emplace_back(corner == 1 || corner == 2 ? 1.0f : 0.0f, corner >= 2 ? 1.0f : 0.0f);
            }
            for (const uint32_t index : { 0u, 1u, 2u, 0u, 2u, 3u }) {
                result.indices.emplace_back(baseVertex + index);
            }
        }
        result.vertexCount = static_cast<uint32_t>(result.positions.size());
        result.indexCount = static_cast<uint32_t>(result.indices.size());
        return result;
    }

    static Runtime::AssetPtr<Runtime::Material> EnsureDefaultUnlitMaterial()
    {
        auto& assetManager = Runtime::AssetManager::Get();
        if (AssetExists(defaultUnlitMaterialUri)) {
            return assetManager.SyncLoad<Runtime::Material>(defaultUnlitMaterialUri, Mirror::Class::Get<Runtime::Material>());
        }

        Runtime::AssetPtr<Runtime::Material> material = new Runtime::Material(defaultUnlitMaterialUri);
        material->SetType(Runtime::MaterialType::surface);
        material->SetSource("float4 GetBaseColor()\n{\n    return baseColor;\n}\n");
        Runtime::MaterialFVec4ParameterField baseColorField;
        baseColorField.defaultValue = Common::FVec4(1.0f, 1.0f, 1.0f, 1.0f);
        material->EmplaceParameterField("baseColor") = baseColorField;
        material->Update();
        SaveAsset(material);
        return material;
    }

    static Runtime::AssetPtr<Runtime::MaterialInstance> EnsureDefaultUnlitMaterialInstance(
        const Core::Uri& inUri,
        const Common::FVec4& inBaseColor,
        const Runtime::AssetPtr<Runtime::Material>& inMaterial)
    {
        auto& assetManager = Runtime::AssetManager::Get();
        if (AssetExists(inUri)) {
            return assetManager.SyncLoad<Runtime::MaterialInstance>(inUri, Mirror::Class::Get<Runtime::MaterialInstance>());
        }

        Runtime::AssetPtr<Runtime::MaterialInstance> materialInstance = new Runtime::MaterialInstance(inUri);
        materialInstance->SetMaterial(inMaterial);
        materialInstance->SetParameter("baseColor", inBaseColor);
        SaveAsset(materialInstance);
        return materialInstance;
    }

    static Runtime::AssetPtr<Runtime::StaticMesh> EnsureDefaultCubeMesh(
        const Runtime::AssetPtr<Runtime::MaterialInstance>& inMaterial)
    {
        auto& assetManager = Runtime::AssetManager::Get();
        if (AssetExists(cubeMeshUri)) {
            return assetManager.SyncLoad<Runtime::StaticMesh>(cubeMeshUri, Mirror::Class::Get<Runtime::StaticMesh>());
        }

        Runtime::AssetPtr<Runtime::StaticMesh> mesh = new Runtime::StaticMesh(cubeMeshUri);
        mesh->SetMaterial(inMaterial);
        mesh->EmplaceLOD().vertices = BuildCubeVertices();
        SaveAsset(mesh);
        return mesh;
    }

    static void AuthorDefaultLevelContent(Runtime::ECRegistry& inRegistry)
    {
        const Runtime::AssetPtr<Runtime::Material> defaultUnlitMaterial = EnsureDefaultUnlitMaterial();
        const Runtime::AssetPtr<Runtime::MaterialInstance> lightGrayMaterial = EnsureDefaultUnlitMaterialInstance(
            lightGrayUnlitMaterialInstanceUri,
            Common::FVec4(0.75f, 0.75f, 0.75f, 1.0f),
            defaultUnlitMaterial);
        const Runtime::AssetPtr<Runtime::MaterialInstance> yellowMaterial = EnsureDefaultUnlitMaterialInstance(
            yellowUnlitMaterialInstanceUri,
            Common::FVec4(1.0f, 1.0f, 0.0f, 1.0f),
            defaultUnlitMaterial);
        const Runtime::AssetPtr<Runtime::StaticMesh> cubeMesh = EnsureDefaultCubeMesh(yellowMaterial);

        // WorldTransform's reflected constructor takes an FTransform, other argument shapes would not match it
        const auto ground = inRegistry.Create();
        inRegistry.Emplace<Runtime::Name>(ground, std::string("Ground"));
        Common::FTransform groundTransform;
        groundTransform.scale = Common::FVec3(10.0f, 10.0f, 0.2f);
        groundTransform.translation = Common::FVec3(0.0f, 0.0f, -0.1f);
        inRegistry.Emplace<Runtime::WorldTransform>(ground, groundTransform);
        auto& groundPrimitive = inRegistry.Emplace<Runtime::StaticPrimitive>(ground);
        groundPrimitive.mesh = cubeMesh;
        groundPrimitive.materialOverride = lightGrayMaterial;

        const auto cube = inRegistry.Create();
        inRegistry.Emplace<Runtime::Name>(cube, std::string("Cube"));
        Common::FTransform cubeTransform;
        cubeTransform.translation = Common::FVec3(0.0f, 0.0f, 0.5f);
        inRegistry.Emplace<Runtime::WorldTransform>(cube, cubeTransform);
        auto& cubePrimitive = inRegistry.Emplace<Runtime::StaticPrimitive>(cube);
        cubePrimitive.mesh = cubeMesh;
        cubePrimitive.materialOverride = yellowMaterial;

        const auto playerStart = inRegistry.Create();
        inRegistry.Emplace<Runtime::Name>(playerStart, std::string("Player Start"));
        const Common::FTransform playerStartTransform = Common::FTransform::LookAt(Common::FVec3(-5.0f, -6.0f, 4.0f), Common::FVec3(0.0f, 0.0f, 0.5f));
        inRegistry.Emplace<Runtime::WorldTransform>(playerStart, playerStartTransform);
        inRegistry.Emplace<Runtime::PlayerStart>(playerStart);
    }
}

namespace Editor {
    SceneClient::SceneClient()
        : levelUri(Internal::mainLevelUri)
        , world("EditorWorld", this, Runtime::PlayType::editor)
        , window(nullptr)
        , renderSurface(nullptr)
        , editorCamera(Runtime::entityNull)
        , sceneHovered(false)
    {
        world.SetSystemGraph(EditorSystemGraphPresets::DefaultEditorWorld());
    }

    SceneClient::~SceneClient()
    {
        EndCameraLook();
        if (!world.Stopped()) {
            world.Stop();
        }
    }

    Runtime::World& SceneClient::GetWorld()
    {
        return world;
    }

    Runtime::RenderSurface* SceneClient::GetRenderSurface()
    {
        return renderSurface;
    }

    void SceneClient::SetEditorWindow(EditorWindow& inWindow)
    {
        window = &inWindow;
    }

    void SceneClient::SetRenderSurface(Runtime::RenderSurface* inRenderSurface)
    {
        renderSurface = inRenderSurface;
    }

    void SceneClient::ResizeRenderSurface(uint32_t inWidth, uint32_t inHeight)
    {
        if (renderSurface == nullptr) {
            return;
        }
        if (const auto* texture = renderSurface->GetTexture();
            texture != nullptr
            && texture->GetCreateInfo().width == inWidth
            && texture->GetCreateInfo().height == inHeight) {
            return;
        }
        if (window != nullptr) {
            window->WaitRenderingIdle();
        }
        renderSurface->Resize(inWidth, inHeight);
    }

    void SceneClient::OpenProjectLevel()
    {
        Assert(world.Stopped());
        if (Internal::AssetExists(levelUri)) {
            const auto level = Runtime::AssetManager::Get().SyncLoad<Runtime::Level>(levelUri, Mirror::Class::Get<Runtime::Level>());
            world.LoadFrom(level);
        } else {
            world.EditorAccess(Internal::AuthorDefaultLevelContent);
            SaveLevel();
        }
        CreateEditorCamera();
        world.Play();
    }

    void SceneClient::SaveLevel()
    {
        Runtime::AssetPtr<Runtime::Level> level = new Runtime::Level(levelUri);
        world.SaveTo(level);
        Internal::SaveAsset(level);
    }

    Runtime::Entity SceneClient::GetEditorCamera() const
    {
        return editorCamera;
    }

    void SceneClient::SetSceneHovered(bool inHovered)
    {
        sceneHovered = inHovered;
    }

    void SceneClient::SetSceneFocused(bool inFocused)
    {
        if (!inFocused && !IsCameraLooking() && world.Playing()) {
            world.EditorAccess([](Runtime::ECRegistry& registry) -> void {
                registry.GUpdate<EditorCameraInput>([](EditorCameraInput& input) -> void {
                    input.moveForward = false;
                    input.moveBackward = false;
                    input.moveLeft = false;
                    input.moveRight = false;
                });
            });
        }
    }

    bool SceneClient::IsSceneHovered() const
    {
        return sceneHovered;
    }

    bool SceneClient::IsCameraLooking()
    {
        if (!world.Playing()) {
            return false;
        }
        bool looking = false;
        world.EditorAccess([&](Runtime::ECRegistry& registry) -> void {
            looking = registry.GHas<EditorCameraInput>()
                && registry.GGet<EditorCameraInput>().looking;
        });
        return looking;
    }

    void SceneClient::OnKey(int inKey, bool inPressed)
    {
        if (!world.Playing()) {
            return;
        }
        if (inKey != GLFW_KEY_W && inKey != GLFW_KEY_S && inKey != GLFW_KEY_A && inKey != GLFW_KEY_D) {
            return;
        }
        world.EditorAccess([&](Runtime::ECRegistry& registry) -> void {
            if (!registry.GHas<EditorCameraInput>()) {
                return;
            }
            registry.GUpdate<EditorCameraInput>([&](EditorCameraInput& input) -> void {
                if (inKey == GLFW_KEY_W) { input.moveForward = inPressed; }
                if (inKey == GLFW_KEY_S) { input.moveBackward = inPressed; }
                if (inKey == GLFW_KEY_A) { input.moveLeft = inPressed; }
                if (inKey == GLFW_KEY_D) { input.moveRight = inPressed; }
            });
        });
    }

    void SceneClient::BeginCameraLook()
    {
        if (IsCameraLooking() || !sceneHovered || !world.Playing()) {
            return;
        }
        world.EditorAccess([](Runtime::ECRegistry& registry) -> void {
            registry.GUpdate<EditorCameraInput>([](EditorCameraInput& input) -> void {
                input.looking = true;
                input.lookDelta = Common::FVec2Consts::zero;
            });
        });
        if (window != nullptr) {
            window->SetCursorCaptured(true);
        }
    }

    void SceneClient::EndCameraLook()
    {
        if (!IsCameraLooking()) {
            return;
        }
        world.EditorAccess([](Runtime::ECRegistry& registry) -> void {
            registry.GUpdate<EditorCameraInput>([](EditorCameraInput& input) -> void {
                input.moveForward = false;
                input.moveBackward = false;
                input.moveLeft = false;
                input.moveRight = false;
                input.looking = false;
                input.lookDelta = Common::FVec2Consts::zero;
            });
        });
        if (window != nullptr) {
            window->SetCursorCaptured(false);
        }
    }

    void SceneClient::AddCameraLookDelta(float inDeltaX, float inDeltaY)
    {
        if (!IsCameraLooking()) {
            return;
        }
        world.EditorAccess([&](Runtime::ECRegistry& registry) -> void {
            registry.GUpdate<EditorCameraInput>([&](EditorCameraInput& input) -> void {
                input.lookDelta += Common::FVec2(inDeltaX, inDeltaY);
            });
        });
    }

    void SceneClient::CreateEditorCamera()
    {
        world.EditorAccess([&](Runtime::ECRegistry& registry) -> void {
            editorCamera = registry.Create();
            registry.AddTag<Runtime::TransientTag>(editorCamera);
            registry.Emplace<Runtime::Camera>(editorCamera);
            registry.Emplace<EditorCameraController>(editorCamera);
            const Common::FTransform cameraTransform = Common::FTransform::LookAt(Common::FVec3(-5.0f, -6.0f, 4.0f), Common::FVec3(0.0f, 0.0f, 0.5f));
            registry.Emplace<Runtime::WorldTransform>(editorCamera, cameraTransform);
        });
    }

}
