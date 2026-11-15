#include <format>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Editor/EditorContext.h>
#include <Editor/System/Camera.h>
#include <Editor/Tab/OutlinerTab.h>
#include <Editor/Widget/IconWidgets.h>
#include <Editor/Widget/TablerIcons.h>
#include <Runtime/Component/Name.h>

namespace Editor::Outliner::Internal {
    static std::string EntityDisplayName(const Runtime::ECRegistry& inRegistry, Runtime::Entity inEntity)
    {
        const auto* name = inRegistry.Find<Runtime::Name>(inEntity);
        return name != nullptr && !name->value.empty()
            ? name->value
            : std::format("Entity {}", inEntity);
    }
}

namespace Editor {
    OutlinerTab::OutlinerTab(EditorContext& inContext)
        : EditorTab(EditorTabDesc {
            .id = "Outliner",
            .title = "Outliner",
            .icon = Icons::Tabler::hierarchy2,
            .defaultDockArea = TabDockArea::rightTop
        })
        , context(inContext)
        , createEntityName("Entity")
    {
    }

    void OutlinerTab::RenderContents()
    {
        context.GetSceneClient().GetWorld().EditorAccess([this](Runtime::ECRegistry& inRegistry) -> void {
            RenderEntities(inRegistry);
        });
    }

    void OutlinerTab::RenderEntities(Runtime::ECRegistry& inRegistry)
    {
        const std::string newEntityLabel = Widgets::Label(Icons::Tabler::plus, "New Entity");
        if (ImGui::Button(newEntityLabel.c_str())) {
            ImGui::OpenPopup("NewEntityMenu");
        }
        ImGui::SetNextWindowSize(ImVec2(260.0f, 0.0f), ImGuiCond_Appearing);
        if (ImGui::BeginPopup("NewEntityMenu")) {
            ImGui::SetNextItemWidth(-1.0f);
            ImGui::InputText("##EntityName", &createEntityName);
            const std::string createLabel = Widgets::Label(Icons::Tabler::check, "Create");
            if (ImGui::Button(createLabel.c_str(), ImVec2(ImGui::GetContentRegionAvail().x, 0.0f))) {
                const auto entity = context.CreateEntity(inRegistry, createEntityName);
                context.SetSelectedEntity(entity);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();

        Runtime::Entity entityToDelete = Runtime::entityNull;
        inRegistry.Each([&](Runtime::Entity entity) -> void {
            if (inRegistry.Has<EditorCameraController>(entity)) {
                return;
            }
            const std::string label = Widgets::Label(Icons::Tabler::box, Outliner::Internal::EntityDisplayName(inRegistry, entity));
            const bool selected = context.GetSelectedEntity() == entity;
            ImGui::PushID(static_cast<int>(entity));
            if (ImGui::Selectable(label.c_str(), selected)) {
                context.SetSelectedEntity(entity);
            }
            if (ImGui::BeginPopupContextItem("EntityMenu")) {
                context.SetSelectedEntity(entity);
                const std::string deleteLabel = Widgets::Label(Icons::Tabler::trash, "Delete");
                if (ImGui::MenuItem(deleteLabel.c_str())) {
                    entityToDelete = entity;
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        });
        if (entityToDelete != Runtime::entityNull) {
            context.DestroyEntity(inRegistry, entityToDelete);
        }
    }
}
