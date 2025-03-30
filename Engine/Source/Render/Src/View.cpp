//
// Created by johnk on 2025/1/8.
//

#include <Render/View.h>

namespace Render {
    ViewData::ViewData()
        : viewMatrix(Common::FMat4x4Consts::identity)
        , projectionMatrix(Common::FMat4x4Consts::identity)
    {
    }

    ViewUniform::ViewUniform(const ViewData& inViewData, const ViewData& inPrevViewData)
        : origin(inViewData.origin)
        , worldToViewMatrix(inViewData.viewMatrix)
        , viewToWorldMatrix(worldToViewMatrix.Inverse())
        , viewToClipMatrix(inViewData.projectionMatrix)
        , clipToViewMatrix(viewToClipMatrix.Inverse())
        , worldToClipMatrix(viewToClipMatrix * worldToViewMatrix)
        , clipToWorldMatrix(worldToClipMatrix.Inverse())
        , prevOrigin(inViewData.origin)
        , prevWorldToViewMatrix(inPrevViewData.viewMatrix)
        , prevViewToWorldMatrix(prevWorldToViewMatrix.Inverse())
        , prevViewToClipMatrix(inPrevViewData.projectionMatrix)
        , prevClipToViewMatrix(prevViewToClipMatrix.Inverse())
        , prevWorldToClipMatrix(prevViewToClipMatrix * prevWorldToViewMatrix)
        , prevClipToWorldMatrix(prevWorldToClipMatrix.Inverse())
    {
    }

    ViewState::ViewState() = default;

    View::View()
        : state(nullptr)
    {
    }
}