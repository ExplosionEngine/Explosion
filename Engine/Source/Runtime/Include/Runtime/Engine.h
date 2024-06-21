//
// Created by johnk on 2024/4/7.
//

#pragma once

namespace Runtime {
    struct EngineInitParams {
        // TODO
    };

    class Engine {
    public:
        virtual ~Engine();

    protected:
        explicit Engine(const EngineInitParams& inParams);
    };

    class GameEngine : public Engine {
    public:
        virtual ~GameEngine();

    protected:
        friend class RuntimeModule;

        explicit GameEngine(const EngineInitParams& inParams);
    };
}
