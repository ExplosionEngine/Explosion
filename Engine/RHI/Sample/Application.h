//
// Created by johnk on 11/1/2022.
//

#ifndef EXPLOSION_RHI_SAMPLE_APPLICATION_H
#define EXPLOSION_RHI_SAMPLE_APPLICATION_H

#include <string>
#include <utility>
#include <GLFW/glfw3.h>
#include <Common/Utility.h>

class Application {
public:
    NON_COPYABLE(Application)
    Application(std::string n, const uint32_t w, const uint32_t h) : window(nullptr), name(std::move(n)), width(w), height(h) {}
    virtual ~Application() = default;

    int Run()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        window = glfwCreateWindow(static_cast<int>(width), static_cast<int>(height), name.c_str(), nullptr, nullptr);

        OnCreate();
        while (!glfwWindowShouldClose(window)) {
            OnDrawFrame();
            glfwPollEvents();
        }
        OnDestroy();

        glfwTerminate();
        return 0;
    }

protected:
    virtual void OnCreate() = 0;
    virtual void OnDestroy() = 0;
    virtual void OnDrawFrame() = 0;

private:
    GLFWwindow* window;
    std::string name;
    uint32_t width;
    uint32_t height;
};

#endif //EXPLOSION_RHI_SAMPLE_APPLICATION_H
