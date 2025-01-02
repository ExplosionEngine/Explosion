//
// Created by johnk on 2024/6/23.
//

#pragma once

#include <Runtime/Engine.h>
#include <Render/RenderModule.h>

namespace Editor {
    class Core {
    public:
        static Core& Get();

        ~Core();

        void Initialize(int argc, char** argv);
        void Cleanup();
        bool ProjectHasSet() const;
        Runtime::Engine* GetEngine() const;

    private:
        Core();

        void ParseCommandLineArgs(int argc, char** argv) const;
        void InitializeRuntime();

        Runtime::Engine* engine;
    };
}
