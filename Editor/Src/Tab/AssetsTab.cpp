//
// Created by johnk on 2026/7/18.
//

#include <format>
#include <system_error>

#include <imgui.h>
#include <imgui_stdlib.h>

#include <Core/Log.h>
#include <Editor/Tab/AssetsTab.h>
#include <Editor/Utils/PlatformUtils.h>
#include <Editor/Widget/IconWidgets.h>
#include <Editor/Widget/TablerIcons.h>

namespace Editor::Assets::Internal {
    constexpr float assetsFolderTreeWidth = 220.0f;
    constexpr const char* assetPathPayload = "EXPLOSION_ASSET_PATH";

    static std::string FormatFileSize(uintmax_t inSize)
    {
        constexpr uintmax_t kibibyte = 1024;
        constexpr uintmax_t mebibyte = kibibyte * 1024;
        constexpr uintmax_t gibibyte = mebibyte * 1024;
        if (inSize >= gibibyte) {
            return std::format("{:.1f} GiB", static_cast<double>(inSize) / static_cast<double>(gibibyte));
        }
        if (inSize >= mebibyte) {
            return std::format("{:.1f} MiB", static_cast<double>(inSize) / static_cast<double>(mebibyte));
        }
        if (inSize >= kibibyte) {
            return std::format("{:.1f} KiB", static_cast<double>(inSize) / static_cast<double>(kibibyte));
        }
        return std::format("{} B", inSize);
    }

    static std::string EntryType(const AssetFileEntry& inEntry)
    {
        if (inEntry.directory) {
            return "Folder";
        }
        std::string extension = inEntry.path.extension().string();
        if (!extension.empty() && extension.front() == '.') {
            extension.erase(extension.begin());
        }
        return extension.empty() ? "File" : extension;
    }
}

namespace Editor {
    AssetsTab::AssetsTab(Common::Path inAssetDirectory)
        : EditorTab(EditorTabDesc {
            .id = "Assets",
            .title = "Assets",
            .icon = Icons::Tabler::folder,
            .defaultDockArea = TabDockArea::bottom
        })
        , fileSystem(inAssetDirectory.String())
        , currentDirectory(fileSystem.Root())
        , clipboardMode(ClipboardMode::none)
        , statusIsError(false)
        , requestCreateFolderPopup(false)
        , requestRenamePopup(false)
        , requestDeletePopup(false)
    {
    }

    void AssetsTab::RenderContents()
    {
        std::error_code error;
        if (!std::filesystem::is_directory(currentDirectory, error) || !fileSystem.Contains(currentDirectory)) {
            NavigateTo(fileSystem.Root());
        }
        if (!selectedPath.empty() && !std::filesystem::exists(selectedPath, error)) {
            selectedPath.clear();
        }
        if (!clipboardPath.empty() && !std::filesystem::exists(clipboardPath, error)) {
            clipboardPath.clear();
            clipboardMode = ClipboardMode::none;
        }

        HandleKeyboardShortcuts();
        RenderToolbar();
        RenderBreadcrumbs();
        if (!statusMessage.empty()) {
            const std::string statusLabel = Widgets::Label(statusIsError ? Icons::Tabler::circleX : Icons::Tabler::infoCircle, statusMessage);
            if (statusIsError) {
                Widgets::PushErrorTextColor();
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_TextDisabled));
            }
            ImGui::TextWrapped("%s", statusLabel.c_str());
            ImGui::PopStyleColor();
        }
        ImGui::Separator();

        ImGui::BeginChild("AssetFolders", ImVec2(Assets::Internal::assetsFolderTreeWidth, 0.0f), ImGuiChildFlags_Borders | ImGuiChildFlags_ResizeX);
        RenderDirectoryTree(fileSystem.Root(), GetDesc().title.c_str());
        ImGui::EndChild();
        ImGui::SameLine();
        ImGui::BeginChild("AssetContents", ImVec2(0.0f, 0.0f), ImGuiChildFlags_Borders);
        RenderDirectoryContents();
        ImGui::EndChild();

        RenderModals();
    }

    void AssetsTab::RenderToolbar()
    {
        const bool atRoot = currentDirectory == fileSystem.Root();
        ImGui::BeginDisabled(atRoot);
        if (Widgets::IconButton("Up", Icons::Tabler::arrowUp, "Up (Backspace)")) {
            NavigateTo(currentDirectory.parent_path());
        }
        ImGui::EndDisabled();
        ImGui::SameLine();
        if (Widgets::IconButton("New Folder", Icons::Tabler::folderPlus, "New Folder")) {
            OpenCreateFolderPopup();
        }
        ImGui::SameLine();
        if (Widgets::IconButton("Import", Icons::Tabler::upload, "Import")) {
            ImportFiles();
        }
        ImGui::SameLine();

        const bool hasSelection = !selectedPath.empty();
        ImGui::BeginDisabled(!hasSelection);
        if (Widgets::IconButton("Cut", Icons::Tabler::cut, "Cut (Ctrl+X)")) {
            SetClipboard(selectedPath, ClipboardMode::move);
        }
        ImGui::SameLine();
        if (Widgets::IconButton("Copy", Icons::Tabler::copy, "Copy (Ctrl+C)")) {
            SetClipboard(selectedPath, ClipboardMode::copy);
        }
        ImGui::SameLine();
        if (Widgets::IconButton("Rename", Icons::Tabler::edit, "Rename (F2)")) {
            OpenRenamePopup(selectedPath);
        }
        ImGui::SameLine();
        if (Widgets::IconButton("Delete", Icons::Tabler::trash, "Delete")) {
            OpenDeletePopup(selectedPath);
        }
        ImGui::EndDisabled();
        ImGui::SameLine();

        ImGui::BeginDisabled(clipboardMode == ClipboardMode::none);
        if (Widgets::IconButton("Paste", Icons::Tabler::clipboard, "Paste (Ctrl+V)")) {
            PasteInto(currentDirectory);
        }
        ImGui::EndDisabled();
    }

    void AssetsTab::RenderBreadcrumbs()
    {
        if (Widgets::IconButton("Asset Root", Icons::Tabler::home, GetDesc().title.c_str())) {
            NavigateTo(fileSystem.Root());
        }

        std::filesystem::path accumulated = fileSystem.Root();
        const std::filesystem::path relative = currentDirectory.lexically_relative(fileSystem.Root());
        for (const auto& component : relative) {
            if (component == ".") {
                continue;
            }
            accumulated /= component;
            ImGui::SameLine();
            ImGui::TextDisabled("%s", Icons::Tabler::chevronRight);
            ImGui::SameLine();
            const std::string id = accumulated.string();
            const std::string label = component.string();
            ImGui::PushID(id.c_str());
            if (ImGui::Button(label.c_str())) {
                NavigateTo(accumulated);
            }
            ImGui::PopID();
        }
    }

    void AssetsTab::RenderDirectoryTree(const std::filesystem::path& inDirectory, const char* inLabel)
    {
        ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (inDirectory == currentDirectory) {
            flags |= ImGuiTreeNodeFlags_Selected;
        }
        if (inDirectory == fileSystem.Root()) {
            flags |= ImGuiTreeNodeFlags_DefaultOpen;
        }

        const std::string id = inDirectory.string();
        const std::string label = Widgets::Label(Icons::Tabler::folder, inLabel);
        ImGui::PushID(id.c_str());
        const bool open = ImGui::TreeNodeEx(label.c_str(), flags);
        if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen()) {
            NavigateTo(inDirectory);
        }
        HandleDropTarget(inDirectory);
        if (open) {
            std::string error;
            for (const AssetFileEntry& entry : fileSystem.List(inDirectory, error)) {
                if (entry.directory) {
                    const std::string label = entry.path.filename().string();
                    RenderDirectoryTree(entry.path, label.c_str());
                }
            }
            if (!error.empty()) {
                ReportResult(error, true);
            }
            ImGui::TreePop();
        }
        ImGui::PopID();
    }

    void AssetsTab::RenderDirectoryContents()
    {
        if (ImGui::BeginTable("AssetEntries", 3, ImGuiTableFlags_BordersInnerV | ImGuiTableFlags_RowBg | ImGuiTableFlags_Resizable | ImGuiTableFlags_ScrollY)) {
            ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 100.0f);
            ImGui::TableHeadersRow();

            std::string error;
            for (const AssetFileEntry& entry : fileSystem.List(currentDirectory, error)) {
                RenderEntry(entry);
            }
            if (!error.empty()) {
                ReportResult(error, true);
            }
            ImGui::EndTable();
        }
        RenderBackgroundMenu();
    }

    void AssetsTab::RenderEntry(const AssetFileEntry& inEntry)
    {
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        const std::string pathId = inEntry.path.string();
        const std::string name = inEntry.path.filename().string();
        const std::string label = Widgets::Label(inEntry.directory ? Icons::Tabler::folder : Icons::Tabler::file, name);
        ImGui::PushID(pathId.c_str());
        const bool selected = selectedPath == inEntry.path;
        if (ImGui::Selectable(label.c_str(), selected, ImGuiSelectableFlags_SpanAllColumns | ImGuiSelectableFlags_AllowDoubleClick)) {
            selectedPath = inEntry.path;
            if (inEntry.directory && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left)) {
                NavigateTo(inEntry.path);
            }
        }

        if (ImGui::BeginDragDropSource()) {
            ImGui::SetDragDropPayload(Assets::Internal::assetPathPayload, pathId.c_str(), pathId.size() + 1);
            ImGui::TextUnformatted(label.c_str());
            ImGui::EndDragDropSource();
        }
        if (inEntry.directory) {
            HandleDropTarget(inEntry.path);
        }
        RenderEntryContextMenu(inEntry, "AssetEntryMenu");

        ImGui::TableSetColumnIndex(1);
        ImGui::TextUnformatted(Assets::Internal::EntryType(inEntry).c_str());
        ImGui::TableSetColumnIndex(2);
        if (inEntry.directory) {
            ImGui::TextDisabled("--");
        } else {
            ImGui::TextUnformatted(Assets::Internal::FormatFileSize(inEntry.size).c_str());
        }
        ImGui::PopID();
    }

    void AssetsTab::RenderEntryContextMenu(const AssetFileEntry& inEntry, const char* inPopupId)
    {
        if (!ImGui::BeginPopupContextItem(inPopupId)) {
            return;
        }
        selectedPath = inEntry.path;
        const std::string openLabel = Widgets::Label(Icons::Tabler::externalLink, "Open");
        if (ImGui::MenuItem(openLabel.c_str(), nullptr, false, inEntry.directory)) {
            NavigateTo(inEntry.path);
        }
        ImGui::Separator();
        const std::string cutLabel = Widgets::Label(Icons::Tabler::cut, "Cut");
        if (ImGui::MenuItem(cutLabel.c_str(), "Ctrl+X")) {
            SetClipboard(inEntry.path, ClipboardMode::move);
        }
        const std::string copyLabel = Widgets::Label(Icons::Tabler::copy, "Copy");
        if (ImGui::MenuItem(copyLabel.c_str(), "Ctrl+C")) {
            SetClipboard(inEntry.path, ClipboardMode::copy);
        }
        const std::string renameLabel = Widgets::Label(Icons::Tabler::edit, "Rename");
        if (ImGui::MenuItem(renameLabel.c_str(), "F2")) {
            OpenRenamePopup(inEntry.path);
        }
        const std::string deleteLabel = Widgets::Label(Icons::Tabler::trash, "Delete");
        if (ImGui::MenuItem(deleteLabel.c_str(), "Delete")) {
            OpenDeletePopup(inEntry.path);
        }
        ImGui::EndPopup();
    }

    void AssetsTab::RenderBackgroundMenu()
    {
        if (!ImGui::BeginPopupContextWindow("AssetBackgroundMenu", ImGuiPopupFlags_MouseButtonRight | ImGuiPopupFlags_NoOpenOverItems)) {
            return;
        }
        const std::string newFolderLabel = Widgets::Label(Icons::Tabler::folderPlus, "New Folder");
        if (ImGui::MenuItem(newFolderLabel.c_str())) {
            OpenCreateFolderPopup();
        }
        const std::string importLabel = Widgets::Label(Icons::Tabler::upload, "Import");
        if (ImGui::MenuItem(importLabel.c_str())) {
            ImportFiles();
        }
        ImGui::Separator();
        const std::string pasteLabel = Widgets::Label(Icons::Tabler::clipboard, "Paste");
        if (ImGui::MenuItem(pasteLabel.c_str(), "Ctrl+V", false, clipboardMode != ClipboardMode::none)) {
            PasteInto(currentDirectory);
        }
        ImGui::EndPopup();
    }

    void AssetsTab::RenderModals()
    {
        if (requestCreateFolderPopup) {
            ImGui::OpenPopup("Create Folder");
            requestCreateFolderPopup = false;
        }
        if (ImGui::BeginPopupModal("Create Folder", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::IsWindowAppearing()) {
                ImGui::SetKeyboardFocusHere();
            }
            ImGui::SetNextItemWidth(320.0f);
            const bool submit = ImGui::InputText("Name", &createFolderName, ImGuiInputTextFlags_EnterReturnsTrue);
            const std::string createLabel = Widgets::Label(Icons::Tabler::check, "Create");
            if (submit || ImGui::Button(createLabel.c_str())) {
                std::filesystem::path created;
                std::string error;
                if (fileSystem.CreateFolder(currentDirectory, createFolderName, created, error)) {
                    selectedPath = created;
                    ReportResult(std::format("Created folder '{}'", created.filename().string()), false);
                    ImGui::CloseCurrentPopup();
                } else {
                    ReportResult(error, true);
                }
            }
            ImGui::SameLine();
            const std::string cancelLabel = Widgets::Label(Icons::Tabler::x, "Cancel");
            if (ImGui::Button(cancelLabel.c_str())) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (requestRenamePopup) {
            ImGui::OpenPopup("Rename Asset");
            requestRenamePopup = false;
        }
        if (ImGui::BeginPopupModal("Rename Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            if (ImGui::IsWindowAppearing()) {
                ImGui::SetKeyboardFocusHere();
            }
            ImGui::SetNextItemWidth(320.0f);
            const bool submit = ImGui::InputText("Name", &renameName, ImGuiInputTextFlags_EnterReturnsTrue);
            const std::string renameLabel = Widgets::Label(Icons::Tabler::edit, "Rename");
            if (submit || ImGui::Button(renameLabel.c_str())) {
                std::filesystem::path renamed;
                std::string error;
                if (fileSystem.Rename(renamePath, renameName, renamed, error)) {
                    selectedPath = renamed;
                    if (clipboardPath == renamePath) {
                        clipboardPath = renamed;
                    }
                    ReportResult(std::format("Renamed asset to '{}'", renamed.filename().string()), false);
                    ImGui::CloseCurrentPopup();
                } else {
                    ReportResult(error, true);
                }
            }
            ImGui::SameLine();
            const std::string cancelLabel = Widgets::Label(Icons::Tabler::x, "Cancel");
            if (ImGui::Button(cancelLabel.c_str())) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }

        if (requestDeletePopup) {
            ImGui::OpenPopup("Delete Asset");
            requestDeletePopup = false;
        }
        if (ImGui::BeginPopupModal("Delete Asset", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            ImGui::TextWrapped("Delete '%s'? This cannot be undone.", deletePath.filename().string().c_str());
            const std::string deleteLabel = Widgets::Label(Icons::Tabler::trash, "Delete");
            if (ImGui::Button(deleteLabel.c_str())) {
                std::string error;
                if (fileSystem.Remove(deletePath, error)) {
                    if (clipboardPath == deletePath) {
                        clipboardPath.clear();
                        clipboardMode = ClipboardMode::none;
                    }
                    selectedPath.clear();
                    ReportResult(std::format("Deleted '{}'", deletePath.filename().string()), false);
                    ImGui::CloseCurrentPopup();
                } else {
                    ReportResult(error, true);
                }
            }
            ImGui::SameLine();
            const std::string cancelLabel = Widgets::Label(Icons::Tabler::x, "Cancel");
            if (ImGui::Button(cancelLabel.c_str())) {
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
    }

    void AssetsTab::HandleKeyboardShortcuts()
    {
        if (!ImGui::IsWindowFocused(ImGuiFocusedFlags_RootAndChildWindows) || ImGui::GetIO().WantTextInput) {
            return;
        }

        const bool hasSelection = !selectedPath.empty();
        const bool control = ImGui::GetIO().KeyCtrl;
        if (hasSelection && control && ImGui::IsKeyPressed(ImGuiKey_C, false)) {
            SetClipboard(selectedPath, ClipboardMode::copy);
        }
        if (hasSelection && control && ImGui::IsKeyPressed(ImGuiKey_X, false)) {
            SetClipboard(selectedPath, ClipboardMode::move);
        }
        if (control && ImGui::IsKeyPressed(ImGuiKey_V, false)) {
            PasteInto(currentDirectory);
        }
        if (hasSelection && ImGui::IsKeyPressed(ImGuiKey_F2, false)) {
            OpenRenamePopup(selectedPath);
        }
        if (hasSelection && ImGui::IsKeyPressed(ImGuiKey_Delete, false)) {
            OpenDeletePopup(selectedPath);
        }
        if (ImGui::IsKeyPressed(ImGuiKey_Backspace, false) && currentDirectory != fileSystem.Root()) {
            NavigateTo(currentDirectory.parent_path());
        }
    }

    void AssetsTab::HandleDropTarget(const std::filesystem::path& inDirectory)
    {
        if (!ImGui::BeginDragDropTarget()) {
            return;
        }
        if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(Assets::Internal::assetPathPayload)) {
            const auto* pathText = static_cast<const char*>(payload->Data);
            std::filesystem::path destination;
            std::string error;
            if (fileSystem.Transfer(pathText, inDirectory, AssetFileTransferMode::move, destination, error)) {
                selectedPath = inDirectory == currentDirectory ? destination : std::filesystem::path();
                if (clipboardPath == std::filesystem::path(pathText)) {
                    clipboardPath = destination;
                }
                ReportResult(std::format("Moved '{}'", destination.filename().string()), false);
            } else {
                ReportResult(error, true);
            }
        }
        ImGui::EndDragDropTarget();
    }

    void AssetsTab::NavigateTo(const std::filesystem::path& inDirectory)
    {
        std::error_code error;
        if (!fileSystem.Contains(inDirectory) || !std::filesystem::is_directory(inDirectory, error)) {
            ReportResult("Could not open the requested asset folder", true);
            return;
        }
        currentDirectory = std::filesystem::weakly_canonical(inDirectory, error);
        if (error) {
            currentDirectory = inDirectory.lexically_normal();
        }
        selectedPath.clear();
    }

    void AssetsTab::OpenCreateFolderPopup()
    {
        createFolderName = "New Folder";
        requestCreateFolderPopup = true;
    }

    void AssetsTab::OpenRenamePopup(const std::filesystem::path& inPath)
    {
        if (inPath.empty()) {
            return;
        }
        renamePath = inPath;
        renameName = inPath.filename().string();
        requestRenamePopup = true;
    }

    void AssetsTab::OpenDeletePopup(const std::filesystem::path& inPath)
    {
        if (inPath.empty()) {
            return;
        }
        deletePath = inPath;
        requestDeletePopup = true;
    }

    void AssetsTab::SetClipboard(const std::filesystem::path& inPath, ClipboardMode inMode)
    {
        clipboardPath = inPath;
        clipboardMode = inMode;
        ReportResult(std::format("{} '{}'", inMode == ClipboardMode::copy ? "Copied" : "Cut", inPath.filename().string()), false);
    }

    void AssetsTab::PasteInto(const std::filesystem::path& inDirectory)
    {
        if (clipboardMode == ClipboardMode::none || clipboardPath.empty()) {
            return;
        }

        const std::filesystem::path source = clipboardPath;
        const AssetFileTransferMode mode = clipboardMode == ClipboardMode::copy
            ? AssetFileTransferMode::copy
            : AssetFileTransferMode::move;
        std::filesystem::path destination;
        std::string error;
        if (!fileSystem.Transfer(source, inDirectory, mode, destination, error)) {
            ReportResult(error, true);
            return;
        }

        selectedPath = destination;
        ReportResult(std::format("{} '{}'", mode == AssetFileTransferMode::copy ? "Copied" : "Moved", destination.filename().string()), false);
        if (mode == AssetFileTransferMode::move) {
            clipboardPath.clear();
            clipboardMode = ClipboardMode::none;
        }
    }

    void AssetsTab::ImportFiles()
    {
        const std::vector<std::string> files = PlatformUtils::SelectFiles("Import Assets", currentDirectory.string());
        size_t importedCount = 0;
        for (const std::string& file : files) {
            std::filesystem::path destination;
            std::string error;
            if (!fileSystem.Import(std::filesystem::u8path(file), currentDirectory, destination, error)) {
                ReportResult(error, true);
                return;
            }
            selectedPath = destination;
            importedCount++;
        }
        if (importedCount > 0) {
            ReportResult(std::format("Imported {} asset{}", importedCount, importedCount == 1 ? "" : "s"), false);
        }
    }

    void AssetsTab::ReportResult(const std::string& inMessage, bool inError)
    {
        if (statusMessage == inMessage && statusIsError == inError) {
            return;
        }
        statusMessage = inMessage;
        statusIsError = inError;
        if (inError) {
            LogError(Assets, "{}", inMessage);
        } else {
            LogInfo(Assets, "{}", inMessage);
        }
    }
}
