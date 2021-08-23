//
// Created by LiZhen on 2021/8/22.
//

#include <RHI/Driver.h>
#include <memory>
#include <string>

namespace Explosion {

    struct RenderCreateInfo {
        std::string rhiName;
    };

    class Render {
    public:
        Render(const RenderCreateInfo& ci);
        ~Render() = default;

    private:
        void InitRHI(const RenderCreateInfo& ci);

        void InitRenderThread();

        std::unique_ptr<RHI::Driver> driver;
    };
}