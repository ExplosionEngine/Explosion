//
// Created by johnk on 2024/3/31.
//

#pragma once

namespace Editor {
    class Application {
    public:
        static Application& Get();

        Application();
        ~Application();

        void SetUp(int argc, char* argv[]);

    private:
        void SetupCli(int argc, char* argv[]);
        void SetupProject();
        void SetupRendering();
    };
}
