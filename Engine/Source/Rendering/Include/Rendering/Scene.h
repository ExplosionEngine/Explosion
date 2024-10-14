//
// Created by johnk on 2023/7/22.
//

#pragma once

#include <unordered_set>

#include <Render/Scene.h>

namespace Rendering {
    class Scene final : public Render::IScene {
    public:
        Scene();
        ~Scene() override;

        // TODO AddLight/RemoveLight/PatchLight

    private:
        // TODO use object tool
    };
}
