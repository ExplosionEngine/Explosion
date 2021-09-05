//
// Created by LiZhen on 2021/8/22.
//

#include <memory>
#include <string>
#include <unordered_map>
#include <RHI/Driver.h>
#include <Engine/Render/SceneView.h>
#include <Common/Asyn/NamedThread.h>

namespace Explosion {

    struct RenderCreateInfo {
        std::string rhiName;
    };

    class Render {
    public:
        ~Render();

        static Render* CreateRender(const RenderCreateInfo& ci);

        static void DestroyRender();

        static Render* GetRender();

        void Tick(float time);

    private:
        Render(const RenderCreateInfo& ci);

        void InitRHI(const RenderCreateInfo& ci);

        void InitRenderThread();

        void RenderMain();

        using ScenePtr = std::unique_ptr<SceneView>;

        std::unique_ptr<RHI::Driver> driver;
        std::unordered_map<std::string, ScenePtr> views;
        NamedThread renderThread;
        RHI::CommandBuffer* mainCommandBuffer = nullptr;
    };
}