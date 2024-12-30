//
// Created by johnk on 2024/6/23.
//

#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

#include <Editor/Widget/Launcher.h>
#include <Editor/Widget/moc_Launcher.cpp> // NOLINT
#include <Editor/Resource.h>
#include <Editor/Theme.h>

namespace Editor {
    Launcher::Launcher()
    {
        SetWindowProperties();
        CreateMainRow();
        CreateMenuCol();
    }

    void Launcher::SetWindowProperties()
    {
        setFixedSize(QSize(800, 600));
        setWindowTitle(tr("Launcher"));
        setWindowIcon(QIcon(StaticResources::picLogo));

        QPalette pal(palette());
        pal.setColor(backgroundRole(), ThemeSwitcher::Current().GetColorBackground());
        setPalette(pal);
    }

    void Launcher::CreateMainRow()
    {
        mainRow = new QHBoxLayout();
        mainRow->setContentsMargins(QMargins(50, 50, 50, 50));
        setLayout(mainRow);
    }

    void Launcher::CreateMenuCol() const
    {
        auto* menuCol = new QVBoxLayout();
        mainRow->addLayout(menuCol);

        auto* logoAndVersionRow = new QHBoxLayout();
        menuCol->addLayout(logoAndVersionRow);

        auto* logoLabel = new QLabel();
        logoLabel->setPixmap(QPixmap(StaticResources::picLogo).scaled(64, 64, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setAlignment(Qt::AlignVCenter);
        logoAndVersionRow->addWidget(logoLabel);
    }
}
