//
// Created by johnk on 2023/7/22.
//

#pragma once

#include <Common/Container.h>
#include <Render/SceneProxy/Light.h>

namespace Render {
    template <typename T> using SPPool = Common::TrunkList<T>;
    template <typename T> using SPHandle = typename SPPool<T>::Handle;

    using LightSPPool = SPPool<LightSceneProxy>;
    using LightSPH = SPHandle<LightSceneProxy>;

    // scene only store render thread cpu data copy, gpu data will auto created by render graph and managed by resource pool
    class Scene final {
    public:
        Scene();
        ~Scene();

        NonCopyable(Scene)
        NonMovable(Scene)

        LightSPH AddLight();
        void UpdateLight(const LightSPH& inHandle);
        void RemoveLight(const LightSPH& inHandle);

    private:
        LightSPPool lights;
    };
}
