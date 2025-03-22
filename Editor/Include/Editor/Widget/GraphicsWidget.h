//
// Created by Kindem on 2025/3/16.
//

#pragma once

#include <QWidget>
#include <QTimer>

#include <Common/Memory.h>
#include <RHI/RHI.h>

namespace Editor {
    class GraphicsWidget : public QWidget {
        Q_OBJECT

    public:
        explicit GraphicsWidget(QWidget* inParent = nullptr);
        ~GraphicsWidget() override;

        RHI::Device& GetDevice() const;
        RHI::Surface& GetSurface() const;

    protected:
        QPaintEngine* paintEngine() const override;

        void WaitDeviceIdle() const;

        RHI::Device* device;
        Common::UniquePtr<RHI::Surface> surface;
    };
}
