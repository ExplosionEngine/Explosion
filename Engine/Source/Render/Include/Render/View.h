//
// Created by johnk on 2025/1/8.
//

#pragma once

#include <Common/Math/Matrix.h>
#include <Common/Math/Rect.h>
#include <Render/Shader.h>

namespace Render {
    struct ViewData {
        ViewData();

        Common::FVec3 origin;
        Common::URect viewport;
        Common::FMat4x4 viewMatrix;
        Common::FMat4x4 projectionMatrix;
    };

    struct ALIGN_AS_GPU ViewUniform {
        ViewUniform(const ViewData& inViewData, const ViewData& inPrevViewData);

        Common::FVec3 origin;
        Common::FMat4x4 worldToViewMatrix;
        Common::FMat4x4 viewToWorldMatrix;
        Common::FMat4x4 viewToClipMatrix;
        Common::FMat4x4 clipToViewMatrix;
        Common::FMat4x4 worldToClipMatrix;
        Common::FMat4x4 clipToWorldMatrix;
        Common::FVec3 prevOrigin;
        Common::FMat4x4 prevWorldToViewMatrix;
        Common::FMat4x4 prevViewToWorldMatrix;
        Common::FMat4x4 prevViewToClipMatrix;
        Common::FMat4x4 prevClipToViewMatrix;
        Common::FMat4x4 prevWorldToClipMatrix;
        Common::FMat4x4 prevClipToWorldMatrix;
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
