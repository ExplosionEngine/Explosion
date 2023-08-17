//
// Created by johnk on 2022/8/3.
//

#pragma once

#include <Render/SceneProxy/Light.h>
#include <Render/SceneProxy/Primitive.h>

namespace Render {
    class IScene {
    public:
        virtual void AddLight(ILightSceneProxy* inProxy) = 0;
        virtual void RemoveLight(ILightSceneProxy* inProxy) = 0;
        virtual void AddPrimitive(IPrimitiveSceneProxy* inProxy) = 0;
        virtual void RemovePrimitive(IPrimitiveSceneProxy* inProxy) = 0;
    };
}
