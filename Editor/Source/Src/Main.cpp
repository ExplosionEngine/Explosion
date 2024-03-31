//
// Created by johnk on 2024/3/31.
//

#include <QApplication>
#include <QPushButton>

int main(int argc, char* argv[])
{
    QApplication a(argc, argv);
    QPushButton button("Hello world!", nullptr);
    button.resize(200, 100);
    button.show();
    return QApplication::exec();
}

