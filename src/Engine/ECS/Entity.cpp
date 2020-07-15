#include <Engine/ECS/Entity.h>
using namespace Explosion;

Entity::~Entity()
{
    for (auto child : children) {
        delete(child);
    }
}

Entity * Entity::Create()
{
    return new Entity();
}

void Entity::Destroy(Entity *entity)
{
    delete(entity);
}

void Entity::AddChild(Entity *child)
{
    children.emplace_back(child);
}

void Entity::RemoveChild(Entity *child)
{
    auto iteration = find(children.begin(), children.end(), child);
    if (iteration != children.end()) {
        delete(*iteration);
        children.erase(iteration);
    }
}

Entity* Entity::FindChild(long childId)
{
    for (auto child : children) {
        if (child->id == childId) {
            return child;
        }
    }
    return nullptr;
}

vector<Entity*> Entity::GetChildren()
{
    return children;
}

void Entity::SetParent(Entity *entity)
{
    parent = entity;
}

Entity* Entity::GetParent()
{
    return parent;
}

