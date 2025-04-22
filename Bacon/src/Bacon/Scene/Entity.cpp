#include "bcpch.h"
#include "Entity.h"

namespace Bacon {

    Entity::Entity(entt::entity handle, Scene* scene)
        : m_EntityHandle(handle), m_Scene(scene)
    {
    }

}