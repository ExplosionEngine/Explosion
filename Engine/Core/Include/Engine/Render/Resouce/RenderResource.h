//
// Created by LiZhen on 2021/8/22.
//

#include <unordered_map>

namespace Explosion {

    class RenderResource {
    public:
        RenderResource() = default;
        virtual ~RenderResource() = default;

        virtual void InitRHI(RHI::Driver& device) {}
    };

}