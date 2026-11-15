#include <Editor/SceneClient.h>

namespace Editor {
    SceneClient::SceneClient(Runtime::World& inWorld)
        : world(inWorld)
    {
    }

    SceneClient::~SceneClient() = default;

    Runtime::World& SceneClient::GetWorld()
    {
        return world;
    }
}
