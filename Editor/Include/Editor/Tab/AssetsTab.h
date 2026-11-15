//
// Created by johnk on 2026/7/18.
//

#pragma once

#include <filesystem>
#include <string>

#include <Common/FileSystem.h>
#include <Editor/Asset/AssetFileSystem.h>
#include <Editor/Tab/EditorTab.h>

namespace Editor {
    class AssetsTab final : public EditorTab {
    public:
        explicit AssetsTab(Common::Path inAssetDirectory);

    private:
        enum class ClipboardMode : uint8_t {
            none,
            copy,
            move
        };

        void RenderContents() override;
        void RenderToolbar();
        void RenderBreadcrumbs();
        void RenderDirectoryTree(const std::filesystem::path& inDirectory, const char* inLabel);
        void RenderDirectoryContents();
        void RenderEntry(const AssetFileEntry& inEntry);
        void RenderEntryContextMenu(const AssetFileEntry& inEntry, const char* inPopupId);
        void RenderBackgroundMenu();
        void RenderModals();
        void HandleKeyboardShortcuts();
        void HandleDropTarget(const std::filesystem::path& inDirectory);
        void NavigateTo(const std::filesystem::path& inDirectory);
        void OpenCreateFolderPopup();
        void OpenRenamePopup(const std::filesystem::path& inPath);
        void OpenDeletePopup(const std::filesystem::path& inPath);
        void SetClipboard(const std::filesystem::path& inPath, ClipboardMode inMode);
        void PasteInto(const std::filesystem::path& inDirectory);
        void ImportFiles();
        void ReportResult(const std::string& inMessage, bool inError);

        AssetFileSystem fileSystem;
        std::filesystem::path currentDirectory;
        std::filesystem::path selectedPath;
        std::filesystem::path clipboardPath;
        std::filesystem::path renamePath;
        std::filesystem::path deletePath;
        ClipboardMode clipboardMode;
        std::string createFolderName;
        std::string renameName;
        std::string statusMessage;
        bool statusIsError;
        bool requestCreateFolderPopup;
        bool requestRenamePopup;
        bool requestDeletePopup;
    };
}
