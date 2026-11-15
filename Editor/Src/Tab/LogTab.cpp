#include <format>

#include <Editor/EditorLog.h>
#include <Editor/Tab/LogTab.h>
#include <Editor/Widget/IconWidgets.h>
#include <Editor/Widget/TablerIcons.h>

namespace Editor::Log::Internal {
    static std::string LevelString(Core::LogLevel inLevel)
    {
        switch (inLevel) {
        case Core::LogLevel::verbose:
            return "verbose";
        case Core::LogLevel::info:
            return "info";
        case Core::LogLevel::warning:
            return "warning";
        case Core::LogLevel::error:
            return "error";
        default:
            return "unknown";
        }
    }
}

namespace Editor {
    LogTab::LogTab(EditorLogStream& inLogStream)
        : EditorTab(EditorTabDesc {
            .id = "Log",
            .title = "Log",
            .icon = Icons::Tabler::terminal2,
            .defaultDockArea = TabDockArea::bottom
        })
        , logStream(inLogStream)
    {
    }

    void LogTab::RenderContents()
    {
        const auto entries = logStream.Snapshot();
        const std::string copyLabel = Widgets::Label(Icons::Tabler::copy, "Copy");
        if (ImGui::Button(copyLabel.c_str())) {
            std::string text;
            for (const auto& entry : entries) {
                text += std::format("[{}][{}][{}] {}\n", entry.time, Log::Internal::LevelString(entry.level), entry.tag, entry.content);
            }
            ImGui::SetClipboardText(text.c_str());
        }
        ImGui::Separator();
        ImGui::BeginChild("LogEntries", ImVec2(0.0f, 0.0f), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto& entry : entries) {
            const char* levelIcon = Icons::Tabler::infoCircle;
            if (entry.level == Core::LogLevel::warning) {
                Widgets::PushWarningTextColor();
                levelIcon = Icons::Tabler::alertTriangle;
            } else if (entry.level == Core::LogLevel::error) {
                Widgets::PushErrorTextColor();
                levelIcon = Icons::Tabler::circleX;
            } else {
                ImGui::PushStyleColor(ImGuiCol_Text, ImGui::GetStyleColorVec4(ImGuiCol_Text));
            }
            ImGui::TextUnformatted(std::format("{} [{}][{}][{}] {}", levelIcon, entry.time, Log::Internal::LevelString(entry.level), entry.tag, entry.content).c_str());
            ImGui::PopStyleColor();
        }
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY()) {
            ImGui::SetScrollHereY(1.0f);
        }
        ImGui::EndChild();
    }
}
