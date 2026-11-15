#pragma once

#include <Common/Delegate.h>

namespace Editor {
    class EditorWindow;

    class ImGuiPlatformBackend final {
    public:
        explicit ImGuiPlatformBackend(EditorWindow& inWindow);
        ~ImGuiPlatformBackend();

        NonCopyable(ImGuiPlatformBackend)
        NonMovable(ImGuiPlatformBackend)

        void NewFrame(float inDeltaSeconds) const;

    private:
        EditorWindow& window;
        Common::CallbackHandle keyHandle;
        Common::CallbackHandle charHandle;
        Common::CallbackHandle mouseButtonHandle;
        Common::CallbackHandle cursorHandle;
        Common::CallbackHandle scrollHandle;
        Common::CallbackHandle focusHandle;
    };
}
