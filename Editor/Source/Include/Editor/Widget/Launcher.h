//
// Created by johnk on 2024/6/23.
//

#pragma once

#include <QMainWindow>
#include <QVBoxLayout>

namespace Editor {
    class Launcher final : public QWidget {
        Q_OBJECT

    public:
        Launcher();

        void SetWindowProperties();
        void CreateMainCol();
        void CreateLogoRow() const;

        QVBoxLayout* mainCol;
    };
}
