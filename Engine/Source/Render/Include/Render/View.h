//
// Created by johnk on 2025/1/8.
//

#pragma once

#include <Common/Math/Matrix.h>
#include <Common/Math/Rect.h>

namespace Render {
    struct ViewRenderData {
        ViewRenderData();

        Common::FMat4x4 viewMatrix;
        Common::FMat4x4 projectionMatrix;
        Common::FRect viewport;
    };

    struct ViewState {
        ViewState();

        ViewRenderData prevViewRenderData;
        // TODO frame number ...
        // TODO temporal history ...
    };

    struct View {
        View();

        ViewRenderData renderData;
        ViewState* viewState;
    };
}
