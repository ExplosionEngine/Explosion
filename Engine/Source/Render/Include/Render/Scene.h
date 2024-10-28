//
// Created by johnk on 2023/7/22.
//

#pragma once

#include <Common/Container.h>
#include <Render/SceneProxy/Light.h>

namespace Render {
    template <typename T> using SPPool = Common::TrunkList<T>;
    template <typename T> using SPHandle = typename SPPool<T>::Handle;
    template <typename T> using SPPatcher = std::function<void(T&)>;

    using LightSPPool = SPPool<LightSceneProxy>;
    using LightSPH = SPHandle<LightSceneProxy>;
    using LightSPPatcher = SPPatcher<LightSceneProxy>;

    class Scene final {
    public:
        Scene();
        ~Scene();

        LightSPH AddLight(const LightSceneProxy& inLight);
        void RemoveLight(const LightSPH& inHandle);
        void PatchLight(const LightSPH& inHandle, const LightSPPatcher& inPatcher);

    private:
        LightSPPool lights;
    };
}
