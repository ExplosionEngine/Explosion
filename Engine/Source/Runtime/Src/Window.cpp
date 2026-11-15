#include <Runtime/Window.h>
#include <Runtime/Engine.h>

namespace Runtime {
    Window::Window()
    {
        EngineHolder::Get().MountWindow(this);
    }

    Window::~Window()
    {
        EngineHolder::Get().UnmountWindow(this);
    }
}
