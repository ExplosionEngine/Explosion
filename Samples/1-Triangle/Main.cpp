//
// Created by Administrator on 2021/4/4 0004.
//

#include <Application/Application.h>
#include <Explosion/Core/Engine.h>

#ifdef __APPLE__
#include <TargetConditionals.h>
#endif

#ifdef TARGET_OS_MAC
#define GLFW_EXPOSE_NATIVE_COCOA
#endif
#include <GLFW/glfw3native.h>

class App : public Explosion::Application {
public:
    App(const std::string& name, uint32_t width, uint32_t height) : Application(name, width, height) {}

protected:
    void OnStart() override
    {
#ifdef TARGET_OS_MAC
        renderer = engine->CreateRenderer(glfwGetCocoaWindow(GetWindow()), GetWidth(), GetHeight());
#endif
        // TODO Windows window
    }

    void OnStop() override
    {
        engine->DestroyRenderer(renderer);
    }

    void OnDrawFrame() override
    {

    }

private:
    Explosion::Engine* engine = Explosion::Engine::GetInstance();
    Explosion::Renderer* renderer = nullptr;
};

int main(int argc, char* argv[])
{
    App app("Triangle", 1024, 768);
    app.Run();
    return 0;
}
