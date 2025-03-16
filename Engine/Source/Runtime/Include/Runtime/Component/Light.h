//
// Created by johnk on 2024/10/14.
//

#pragma once

#include <Common/Math/Color.h>
#include <Runtime/Meta.h>
#include <Runtime/Api.h>

namespace Runtime {
    struct RUNTIME_API EClass() DirectionalLight final {
        EClassBody(DirectionalLight)

        DirectionalLight();

        EProperty() Common::Color color;
        EProperty() float intensity;
        EProperty() bool castShadows;
    };

    struct RUNTIME_API EClass() PointLight final {
        EClassBody(PointLight)

        PointLight();

        EProperty() Common::Color color;
        EProperty() float intensity;
        EProperty() bool castShadows;
        EProperty() float radius;
    };

    struct RUNTIME_API EClass() SpotLight final {
        EClassBody(SpotLight)

        SpotLight();

        EProperty() Common::Color color;
        EProperty() float intensity;
        EProperty() bool castShadows;
    };
}
