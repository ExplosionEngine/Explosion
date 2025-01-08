//
// Created by johnk on 2025/1/8.
//

#include <Render/View.h>

namespace Render {
    View::View() = default;

    View::~View() = default;

    void View::Update(const Common::FMat4x4& inViewMatrix, const Common::FMat4x4& inProjectionMatrix)
    {
        viewMatrix = inViewMatrix;
        projectionMatrix = inProjectionMatrix;
        // TODO ...
    }
}
