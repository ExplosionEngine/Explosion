#include <QApplication>
#include <QWebEngineView>
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    QWebEngineView view;
    view.load(QUrl("https://qt-project.org/"));
    view.resize(1024, 768);

    QTimer::singleShot(100, []() -> void { QApplication::quit(); });
    return app.exec();
}