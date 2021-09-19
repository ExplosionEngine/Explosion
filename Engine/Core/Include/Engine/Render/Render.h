//
// Created by Zach Lee on 2021/8/22.
//

#include <memory>
#include <string>
#include <unordered_map>
#include <RHI/Driver.h>
#include <Engine/Render/SceneView.h>
#include <Common/Asyn/NamedThread.h>

namespace Explosion {

    class Render {
    public:
        ~Render();

        struct Descriptor {
            std::string rhiName;
        };

        static Render* CreateRender(const Descriptor& ci);

        static void DestroyRender(Render*);

        RHI::Driver* GetDriver();

        void Tick(float time);

    private:
        Render(const Descriptor& ci);

        void InitRHI(const Descriptor& ci);

        void InitRenderThread();

        void RenderMain();

        using ScenePtr = std::unique_ptr<SceneView>;

        std::unique_ptr<RHI::Driver> driver;
        std::unordered_map<std::string, ScenePtr> views;
        NamedThread renderThread;
        RHI::CommandBuffer* mainCommandBuffer = nullptr;
    };
}