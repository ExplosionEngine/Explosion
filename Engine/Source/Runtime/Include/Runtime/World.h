//
// Created by johnk on 2024/10/31.
//

#pragma once

namespace Runtime {
    class World {
    public:
        NonCopyable(World)
        ~World();

    private:
        friend class Engine;

        explicit World(const std::string& inName = "");
        // TODO play/stop/pause/status

        std::string name;
    };
}

