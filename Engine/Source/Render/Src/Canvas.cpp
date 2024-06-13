//
// Created by johnk on 2022/8/4.
//

#include <Render/Canvas.h>

namespace Render {
    void Canvas::Destroy() const
    {
        delete this;
    }
}
