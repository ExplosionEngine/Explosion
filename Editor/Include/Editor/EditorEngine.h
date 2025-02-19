//
// Created by johnk on 2024/8/21.
//

#pragma once

#include <Runtime/Engine.h>

namespace Editor {
    class EditorEngine final : public Runtime::Engine {
    public:
        ~EditorEngine() override;

        bool IsEditor() override;

    private:
        friend class EditorModule;

        explicit EditorEngine(const Runtime::EngineInitParams& inParams);
    };

    EditorEngine& GetEditorEngine();
}
