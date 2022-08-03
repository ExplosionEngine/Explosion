//
// Created by johnk on 2022/8/3.
//

#include <Launcher/GameApplication.h>

int main(int argc, char* argv[])
{
    Launcher::GameWindow window;
    Launcher::GameApplication application;
    application.SetMainWindow(&window);
    return application.Exec(argc, argv);
}
