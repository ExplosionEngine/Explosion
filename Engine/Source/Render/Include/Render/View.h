//
// Created by johnk on 2025/1/8.
//

#pragma once

#include <Common/Utility.h>
#include <Common/Math/Vector.h>
#include <Common/Math/Matrix.h>
#include <Common/Math/Rect.h>

namespace Render {
    class View {
    public:
        View();
        ~View();

        NonCopyable(View)
        NonMovable(View)

        void Update(const Common::FMat4x4& inViewMatrix, const Common::FMat4x4& inProjectionMatrix, const Common::FRect& inViewport);

    private:
        Common::FMat4x4 viewMatrix;
        Common::FMat4x4 projectionMatrix;
        Common::FRect viewport;
        // TODO viewOrigin, etc...
    };
}
