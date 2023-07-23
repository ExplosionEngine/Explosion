//
// Created by johnk on 2022/11/18.
//

#pragma once

#include <functional>

#include <Common/Math/Transform.h>
#include <Runtime/ECS.h>

namespace Runtime {
    using EntityTraverseFunc = std::function<void(Entity)>;

    class EClass() TransformComponent : public Component {
    public:
        EClassBody(TransformComponent)

        EFunc()
        void TraverseChildren(const EntityTraverseFunc& func) const;

        EFunc()
        void TraverseOffspring(const EntityTraverseFunc& func) const;

        EFunc()
        void TraverseBrothers(const EntityTraverseFunc& func) const;

        EProperty(transient, editorHide)
        Entity parent;

        EProperty(transient, editorHide)
        Entity firstChild;

        EProperty(transient, editorHide)
        Entity nextBrother;

        EProperty(category=Transform)
        Common::FTransform transform;
    };
}
