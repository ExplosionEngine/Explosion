//
// Created by johnk on 2026/7/4.
//

#include <array>

#include <Core/Uri.h>
#include <Editor/EditorContext.h>
#include <Editor/SystemGraphPresets.h>
#include <Runtime/Asset/Material.h>
#include <Runtime/Asset/Mesh.h>
#include <Runtime/Component/Camera.h>
#include <Runtime/Component/Player.h>
#include <Runtime/Component/Primitive.h>
#include <Runtime/Component/Name.h>
#include <Runtime/Component/Transform.h>

namespace Editor::LevelEditing::Internal {
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
    EditorContext::EditorContext()
        : levelUri(LevelEditing::Internal::mainLevelUri)
        , world("EditorWorld", Runtime::PlayType::editor)
        , selectedEntity(Runtime::entityNull)
    {
        world.SetSystemGraph(EditorSystemGraphPresets::DefaultEditorWorld());
    }

    EditorContext::~EditorContext()
    {
        if (!world.Stopped()) {
            world.Stop();
        }
    }

    Runtime::World& EditorContext::GetWorld()
    {
        return world;
    }

    void EditorContext::OpenProjectLevel()
    {
        Assert(world.Stopped());
        if (LevelEditing::Internal::AssetExists(levelUri)) {
            const auto level = Runtime::AssetManager::Get().SyncLoad<Runtime::Level>(levelUri, Mirror::Class::Get<Runtime::Level>());
            world.LoadFrom(level);
        } else {
            world.EditorAccess(LevelEditing::Internal::AuthorDefaultLevelContent);
            SaveLevel();
        }
        world.Play();
    }

    void EditorContext::SaveLevel()
    {
        Runtime::AssetPtr<Runtime::Level> level = new Runtime::Level(levelUri);
        world.SaveTo(level);
        LevelEditing::Internal::SaveAsset(level);
    }

    Runtime::Entity EditorContext::GetSelectedEntity() const
    {
        return selectedEntity;
    }

    bool EditorContext::CanAddComponent(const Runtime::ECRegistry& inRegistry, Runtime::Entity inEntity, Runtime::CompClass inClass) const
    {
        const bool isTag = inClass != nullptr && inClass->HasMeta(Runtime::MetaPresets::tag);
        if (!inRegistry.Valid(inEntity)
            || inClass == nullptr
            || !inClass->HasMeta("comp")
            || (!isTag && !inClass->HasDefaultConstructor())) {
            return false;
        }
        return isTag
            ? !inRegistry.HasTagDyn(inClass, inEntity)
            : !inRegistry.HasDyn(inClass, inEntity);
    }

    bool EditorContext::CanRemoveComponent(const Runtime::ECRegistry& inRegistry, Runtime::Entity inEntity, Runtime::CompClass inClass) const
    {
        if (!inRegistry.Valid(inEntity) || inClass == nullptr || !inClass->HasMeta("comp")) {
            return false;
        }
        return inClass->HasMeta(Runtime::MetaPresets::tag)
            ? inRegistry.HasTagDyn(inClass, inEntity)
            : inRegistry.HasDyn(inClass, inEntity);
    }

    void EditorContext::SetSelectedEntity(Runtime::Entity inEntity)
    {
        if (selectedEntity == inEntity) {
            return;
        }
        selectedEntity = inEntity;
    }

    Runtime::Entity EditorContext::CreateEntity(Runtime::ECRegistry& inRegistry, const std::string& inName)
    {
        const Runtime::Entity entity = inRegistry.Create();
        inRegistry.Emplace<Runtime::Name>(entity, inName);
        inRegistry.Emplace<Runtime::WorldTransform>(entity);
        return entity;
    }

    void EditorContext::DestroyEntity(Runtime::ECRegistry& inRegistry, Runtime::Entity inEntity)
    {
        if (!inRegistry.Valid(inEntity)) {
            return;
        }
        Runtime::HierarchyOps::Destroy(inRegistry, inEntity);
        if (selectedEntity == inEntity) {
            SetSelectedEntity(Runtime::entityNull);
        }
    }

    bool EditorContext::AddComponent(Runtime::ECRegistry& inRegistry, Runtime::Entity inEntity, Runtime::CompClass inClass)
    {
        if (!CanAddComponent(inRegistry, inEntity, inClass)) {
            return false;
        }
        if (inClass->HasMeta(Runtime::MetaPresets::tag)) {
            inRegistry.AddTagDyn(inClass, inEntity);
        } else {
            inRegistry.EmplaceDyn(inClass, inEntity, {});
        }
        return true;
    }

    bool EditorContext::RemoveComponent(Runtime::ECRegistry& inRegistry, Runtime::Entity inEntity, Runtime::CompClass inClass)
    {
        if (!CanRemoveComponent(inRegistry, inEntity, inClass)) {
            return false;
        }
        if (inClass->HasMeta(Runtime::MetaPresets::tag)) {
            inRegistry.RemoveTagDyn(inClass, inEntity);
        } else if (inClass == &Runtime::Hierarchy::GetStaticClass()) {
            Runtime::HierarchyOps::Remove(inRegistry, inEntity);
        } else {
            inRegistry.RemoveDyn(inClass, inEntity);
        }
        return true;
    }

    bool EditorContext::SetComponentMember(Runtime::ECRegistry& inRegistry, Runtime::Entity inEntity, Runtime::CompClass inClass, const Mirror::MemberVariable& inMemberVariable, const Mirror::Any& inValue)
    {
        if (inClass == nullptr
            || !inRegistry.Valid(inEntity)
            || !inRegistry.HasDyn(inClass, inEntity)
            || &inMemberVariable.GetOwner() != inClass) {
            return false;
        }
        inRegistry.UpdateDyn(inClass, inEntity, [&](const Mirror::Any& inComponent) -> void {
            inMemberVariable.SetDyn(inComponent, inValue);
        });
        return true;
    }
}
