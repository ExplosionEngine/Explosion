//
// Created by johnk on 2025/2/19.
//

#include <Launch/GameApplication.h>

int main(int argc, char* argv[])
{
    Launch::GameApplication app(argc, argv);
    while (!app.ShouldClose()) {
        app.Tick();
    }
    return 0;
}
