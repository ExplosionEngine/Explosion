//
// Created by johnk on 2024/6/23.
//

#include <Editor/Widget/ProjectSelector.h>

namespace Editor {
    QProjectSelector::QProjectSelector()
    {
        resize(1024, 768);
        layout = new QVBoxLayout(this);
    }

    QProjectSelector::~QProjectSelector() = default;
}
