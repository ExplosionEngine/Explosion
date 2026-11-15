#include <algorithm>
#include <ranges>

#include <Editor/EditorContext.h>
#include <Editor/Tab/InspectorTab.h>
#include <Editor/Widget/IconWidgets.h>
#include <Editor/Widget/InputWidgets.h>
#include <Editor/Widget/TablerIcons.h>

namespace Editor::Inspector::Internal {
    static std::string ComponentDisplayName(const Mirror::Class& inClass)
    {
        const std::string& qualifiedName = inClass.GetName();
        const size_t namespaceSeparator = qualifiedName.rfind("::");
        return namespaceSeparator == std::string::npos
            ? qualifiedName
            : qualifiedName.substr(namespaceSeparator + 2);
    }

    static bool IsComponentVisibleInDetails(const Mirror::Class& inClass)
    {
        return !inClass.HasMeta(Runtime::MetaPresets::editorHide);
    }
}

namespace Editor {
    InspectorTab::InspectorTab(EditorContext& inContext)
        : EditorTab(EditorTabDesc {
            .id = "Inspector",
            .title = "Inspector",
            .icon = Icons::Tabler::adjustmentsHorizontal,
            .defaultDockArea = TabDockArea::rightBottom
        })
        , context(inContext)
        , componentClasses(Mirror::Class::GetAll())
    {
        std::erase_if(componentClasses, [](Runtime::CompClass inClass) -> bool {
            return !inClass->HasMeta("comp") || !Inspector::Internal::IsComponentVisibleInDetails(*inClass);
        });
        std::ranges::sort(componentClasses, [](Runtime::CompClass lhs, Runtime::CompClass rhs) -> bool {
            return lhs->GetName() < rhs->GetName();
        });
    }

    void InspectorTab::RenderContents()
    {
        context.GetWorld().EditorAccess([this](Runtime::ECRegistry& inRegistry) -> void {
            RenderComponents(inRegistry);
        });
    }

    void InspectorTab::RenderComponents(Runtime::ECRegistry& inRegistry)
    {
        const Runtime::Entity selectedEntity = context.GetSelectedEntity();
        if (selectedEntity == Runtime::entityNull || !inRegistry.Valid(selectedEntity)) {
            ImGui::TextDisabled("No entity selected");
            return;
        }
        ImGui::PushID(static_cast<int>(selectedEntity));

        std::vector<const Mirror::Class*> addableComponents;
        for (const auto* clazz : componentClasses) {
            if (context.CanAddComponent(inRegistry, selectedEntity, clazz)) {
                addableComponents.emplace_back(clazz);
            }
        }

        ImGui::BeginDisabled(addableComponents.empty());
        const std::string addComponentLabel = Widgets::Label(Icons::Tabler::plus, "Add Component");
        if (ImGui::Button(addComponentLabel.c_str())) {
            ImGui::OpenPopup("AddComponentMenu");
        }
        ImGui::EndDisabled();
        if (ImGui::BeginPopup("AddComponentMenu")) {
            for (const auto* clazz : addableComponents) {
                const std::string componentName = Inspector::Internal::ComponentDisplayName(*clazz);
                ImGui::PushID(clazz->GetName().c_str());
                const std::string componentLabel = Widgets::Label(Icons::Tabler::boxMultiple, componentName);
                if (ImGui::MenuItem(componentLabel.c_str())) {
                    context.AddComponent(inRegistry, selectedEntity, clazz);
                }
                ImGui::PopID();
            }
            ImGui::EndPopup();
        }
        ImGui::Separator();

        Runtime::CompClass componentToRemove = nullptr;
        inRegistry.CompEach(selectedEntity, [&](Runtime::CompClass compClass) -> void {
            if (!Inspector::Internal::IsComponentVisibleInDetails(*compClass)) {
                return;
            }
            ImGui::PushID(compClass->GetName().c_str());
            const std::string componentName = Inspector::Internal::ComponentDisplayName(*compClass);
            const std::string componentLabel = Widgets::Label(Icons::Tabler::boxMultiple, componentName);
            const bool open = ImGui::CollapsingHeader(componentLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen);
            if (context.CanRemoveComponent(inRegistry, selectedEntity, compClass) && ImGui::BeginPopupContextItem("ComponentMenu")) {
                const std::string removeLabel = Widgets::Label(Icons::Tabler::trash, "Remove");
                if (ImGui::MenuItem(removeLabel.c_str())) {
                    componentToRemove = compClass;
                }
                ImGui::EndPopup();
            }
            if (open && componentToRemove != compClass) {
                const Mirror::Any compRef = inRegistry.GetDyn(compClass, selectedEntity);
                for (const auto& memberVariable : compClass->GetMemberVariables() | std::views::values) {
                    const Mirror::Any memberRef = memberVariable.GetDyn(compRef);
                    Mirror::Any memberValue = memberRef.Value();
                    if (RenderInputWidget(memberVariable.GetName(), memberValue)) {
                        context.SetComponentMember(inRegistry, selectedEntity, compClass, memberVariable, memberValue);
                    }
                }
            }
            ImGui::PopID();
        });
        inRegistry.TagEach(selectedEntity, [&](Runtime::TagClass tagClass) -> void {
            if (!Inspector::Internal::IsComponentVisibleInDetails(*tagClass)) {
                return;
            }
            ImGui::PushID(tagClass->GetName().c_str());
            const std::string tagName = Inspector::Internal::ComponentDisplayName(*tagClass);
            const std::string tagLabel = Widgets::Label(Icons::Tabler::box, tagName);
            ImGui::TextUnformatted(tagLabel.c_str());
            if (context.CanRemoveComponent(inRegistry, selectedEntity, tagClass) && ImGui::BeginPopupContextItem("ComponentMenu")) {
                const std::string removeLabel = Widgets::Label(Icons::Tabler::trash, "Remove");
                if (ImGui::MenuItem(removeLabel.c_str())) {
                    componentToRemove = tagClass;
                }
                ImGui::EndPopup();
            }
            ImGui::PopID();
        });
        if (componentToRemove != nullptr) {
            context.RemoveComponent(inRegistry, selectedEntity, componentToRemove);
        }
        ImGui::PopID();
    }
}
