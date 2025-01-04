//
// Created by Kindem on 2025/1/5.
//

#include <Editor/Widget/WidgetSamples.h>
#include <Editor/Widget/moc_WidgetSamples.cpp> // NOLINT

namespace Editor {
    WidgetSamples::WidgetSamples()
        : QmlWidget("EWidgetSamples.qml")
    {
        setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);
        setFixedSize(1024, 768);
    }
}
