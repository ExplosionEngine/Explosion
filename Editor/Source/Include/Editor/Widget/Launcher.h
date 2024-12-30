//
// Created by johnk on 2024/6/23.
//

#pragma once

#include <QMainWindow>
#include <QHBoxLayout>

namespace Editor {
    class Launcher final : public QWidget {
        Q_OBJECT

    public:
        Launcher();

        void SetWindowProperties();
        void CreateMainRow();
        void CreateMenuCol() const;

        QHBoxLayout* mainRow;
    };
}
