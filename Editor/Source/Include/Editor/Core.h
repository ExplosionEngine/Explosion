//
// Created by johnk on 2024/6/23.
//

#pragma once

#include <Runtime/Engine.h>
#include <Rendering/RenderingModule.h>

namespace Editor {
    class Core {
    public:
        static Core& Get();

        ~Core();

        void Initialize(int argc, char** argv);
        void Cleanup();
        bool ProjectRooHasSet() const;
        Runtime::RuntimeModule* GetRuntimeModule() const;
        Rendering::RenderingModule* GetRenderingModule() const;
        Runtime::EditorEngine* GetEngine() const;

    private:
        Core();

        void ParseCommandLineArgs(int argc, char** argv) const;
        void InitializeRuntime();
        void InitializeRendering();

        Runtime::RuntimeModule* runtimeModule;
        Rendering::RenderingModule* renderingModule;
        Runtime::EditorEngine* engine;
    };
}
