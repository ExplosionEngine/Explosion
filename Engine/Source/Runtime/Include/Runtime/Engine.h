//
// Created by johnk on 2024/4/7.
//

#pragma once

#include <Runtime/Api.h>

namespace Runtime {
    struct EngineInitParams {
        // TODO
    };

    class RUNTIME_API Engine {
    public:
        virtual ~Engine();

    protected:
        explicit Engine(const EngineInitParams& inParams);
    };

    class RUNTIME_API GameEngine final : public Engine {
    public:
        ~GameEngine() override;

    protected:
        friend class RuntimeModule;

        explicit GameEngine(const EngineInitParams& inParams);
    };

#if BUILD_EDITOR
    class RUNTIME_API EditorEngine final : public Engine {
    public:
        ~EditorEngine() override;

    protected:
        friend class RuntimeModule;

        explicit EditorEngine(const EngineInitParams& inParams);
    };
#endif
}
