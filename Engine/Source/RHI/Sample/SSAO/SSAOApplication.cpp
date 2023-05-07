//
// Created by Junkang on 2023/3/8.
//

#include <vector>
#include <array>
#include <memory>

#include <Application.h>
#include "Renderer.h"

using namespace RHI;

class SSAOApplication : public Application {
public:
    NON_COPYABLE(SSAOApplication)
    explicit SSAOApplication(const std::string& n) : Application(n) {}
    ~SSAOApplication() override = default;

protected:
    void OnCreate() override
    {
        Initialzie();
    }

    void OnDrawFrame() override
    {
        renderer->RenderFrame();
    }

    void OnDestroy() override
    {

    }

private:
    void Initialzie() {
        renderer = std::make_unique<Example::Renderer>(this, rhiType);
        renderer->Initialize();
    }

    std::unique_ptr<Example::Renderer> renderer;

};

int main(int argc, char* argv[])
{
    SSAOApplication application("SSAO");
    return application.Run(argc, argv);
}

