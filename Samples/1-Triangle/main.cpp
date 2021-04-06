//
// Created by Administrator on 2021/4/4 0004.
//

#include <iostream>

#include <Application/Application.h>
#include <Explosion/Engine.h>

class App : public Explosion::Application {
public:
    App(const std::string& name, uint32_t width, uint32_t height) : Application(name, width, height) {}

protected:
    void OnStart() override
    {

    }

    void OnStop() override
    {

    }

    void OnDrawFrame() override
    {

    }

private:
    Explosion::Engine* engine = Explosion::Engine::GetInstance();
};

int main(int argc, char* argv[])
{
    App app("Triangle", 1024, 768);
    app.Run();
    return 0;
}
