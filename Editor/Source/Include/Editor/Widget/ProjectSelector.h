//
// Created by johnk on 2024/6/23.
//

#pragma once

#include <QMainWindow>
#include <QVBoxLayout>

namespace Editor {
    class QProjectSelector final : public QMainWindow {
        Q_OBJECT

    public:
        QProjectSelector();
        ~QProjectSelector() override;

    private:
        QVBoxLayout* layout;
    };
}
