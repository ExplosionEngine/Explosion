//
// Created by johnk on 2022/11/18.
//

#pragma once

#include <string>

#include <Runtime/Component/Component.h>

namespace Runtime {
    class EClass(editorHide) EntityInfoComponent : public Component {
    public:
        EClassBody(EntityInfoComponent)

        ECtor()
        EntityInfoComponent();

    private:
        EProperty()
        std::string name;
    };
}
