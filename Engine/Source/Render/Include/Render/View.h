//
// Created by johnk on 2025/1/8.
//

#pragma once

#include <Common/Math/Matrix.h>
#include <Common/Math/Rect.h>

namespace Render {
    struct ViewData {
        ViewData();

        Common::FMat4x4 viewMatrix;
        Common::FMat4x4 projectionMatrix;
        Common::URect viewport;
    };

    struct ViewState {
        ViewState();

        ViewData prevData;
        // TODO frame number ...
        // TODO temporal history ...
    };

    struct View {
        View();

        ViewData data;
        ViewState* state;
    };
}
