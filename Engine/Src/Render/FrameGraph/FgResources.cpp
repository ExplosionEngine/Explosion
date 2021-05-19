//
// Created by LiZhen on 2021/5/9.
//

#include <Explosion/Render/FrameGraph/FgResources.h>

namespace Explosion {

    void FgTextureSubResource::AddRef()
    {
        RefObject::AddRef();
        parent->AddRef();
    }

    void FgTextureSubResource::RemoveRef()
    {
        parent->RemoveRef();
        RefObject::RemoveRef();
    }

}