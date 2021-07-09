//
// Created by John Kindem on 2021/3/30.
//

#ifndef EXPLOSION_APPLICATION_H
#define EXPLOSION_APPLICATION_H

#include <cstdint>
#include <string>

#include <GLFW/glfw3.h>

namespace Explosion {
    class Application {
    public:
        Application(std::string name, uint32_t width, uint32_t height);
        ~Application();
        void Run();

    protected:
        [[nodiscard]] GLFWwindow* GetWindow() const;
        [[nodiscard]] uint32_t GetWidth() const;
        [[nodiscard]] uint32_t GetHeight() const;

        virtual void OnStart() = 0;
        virtual void OnStop() = 0;
        virtual void OnDrawFrame() = 0;

    private:
        std::string name;
        uint32_t width;
        uint32_t height;
        GLFWwindow* window;
    };
}

#endif //EXPLOSION_APPLICATION_H
