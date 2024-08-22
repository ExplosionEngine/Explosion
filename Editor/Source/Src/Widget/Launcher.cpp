//
// Created by johnk on 2024/6/23.
//

#include <QLabel>
#include <QPushButton>

#include <Editor/Widget/Launcher.h>
#include <Editor/Widget/moc_Launcher.cpp> // NOLINT
#include <Editor/Resource.h>
#include <Editor/Theme.h>

namespace Editor {
    Launcher::Launcher()
    {
        SetWindowProperties();
        CreateMainCol();
        CreateLogoRow();
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

    void Launcher::CreateMainCol()
    {
        mainCol = new QVBoxLayout();
        setLayout(mainCol);
    }

    void Launcher::CreateLogoRow() const
    {
        auto* logoRow = new QHBoxLayout();
        logoRow->setContentsMargins(QMargins(0, 50, 0, 0));
        mainCol->addLayout(logoRow);

        auto* logoLabel = new QLabel();
        logoLabel->setPixmap(QPixmap(StaticResources::picLogo).scaled(250, 250, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        logoLabel->setAlignment(Qt::AlignHCenter);
        mainCol->addWidget(logoLabel);
    }
}
