#ifndef EXPLOSION_ENTITY_H
#define EXPLOSION_ENTITY_H

#include <vector>
using namespace std;

static long entityCount = 0;

namespace Explosion {
class Entity {
private:
    Entity(): id(entityCount++) {}
    ~Entity();

    long id = 0;
    vector<Entity*> children;
    Entity* parent = nullptr;
protected:
public:
    static Entity* Create();
    static void Destroy(Entity* entity);

    void AddChild(Entity* child);
    void RemoveChild(Entity* child);
    Entity* FindChild(long childId);
    vector<Entity*> GetChildren();
    void SetParent(Entity* entity);
    Entity* GetParent();
};
}

#endif