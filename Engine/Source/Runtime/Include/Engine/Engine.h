//
// Created by johnk on 2022/7/18.
//

#pragma once

#include <string>
#include <unordered_map>
#include <memory>

#include <Common/Path.h>

namespace Runtime {
    class Engine {
    public:
        static Engine& Get();

        ~Engine();

        bool Initialize(int argc, char* argv[]);
        [[nodiscard]] const Common::PathMapper& GetPathMapper() const;
        void Tick();

    protected:
        Engine();

    private:
        std::unique_ptr<Common::PathMapper> pathMapper;
    };
}
