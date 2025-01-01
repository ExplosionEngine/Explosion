//
// Created by johnk on 2024/8/21.
//

#include <Editor/EditorEngine.h>

namespace Editor {
    EditorEngine::~EditorEngine() = default;

    bool EditorEngine::IsEditor()
    {
        return true;
    }

    EditorEngine::EditorEngine(const Runtime::EngineInitParams& inParams)
        : Engine(inParams)
    {
    }
}
