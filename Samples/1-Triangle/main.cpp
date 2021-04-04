//
// Created by Administrator on 2021/4/4 0004.
//

#include <iostream>

#include <Application/Application.h>

class App : public Explosion::Application {
public:
    App(const std::string& name, uint32_t width, uint32_t height) : Application(name, width, height) {}

protected:
    void OnStart() override
    {
#ifdef ENABLE_VALIDATION_LAYER
        std::cout << "Hello" << std::endl;
#endif
    }

    void OnStop() override
    {

    }

    void OnDrawFrame() override
    {

    }
};

int main(int argc, char* argv[])
{
    App app("Triangle", 1024, 768);
    app.Run();
    return 0;
}
