#pragma once

#include "Bacon/Core/Timestep.h"
#include "Bacon/Core/UUID.h"
#include "Bacon/Renderer/EditorCamera.h"

#include "entt.hpp"

// Box2D
#include "box2d/box2d.h"

namespace Bacon {

    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

		static Ref<Scene> Copy(Ref<Scene> other);

        Entity CreateEntity(const std::string& name = std::string());
		Entity CreateEntityWithUUID(UUID uuid, const std::string& name = std::string());
        void DestroyEntity(Entity entity);

		void OnRuntimeStart();
		void OnRuntimeStop();

		void OnUpdateRuntime(Timestep ts);
		void OnUpdateEditor(Timestep ts, EditorCamera& camera);
        void OnViewportResize(uint32_t width, uint32_t height);

		void DuplicateEntity(Entity entity);

		Entity GetPrimaryCameraEntity();

		template<typename... Components>
		auto GetAllEntitiesWith()
		{
			return m_Registry.view<Components...>();
		}
    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);
    private:
        entt::registry m_Registry;
        uint32_t m_ViewportWidth = 0, m_ViewportHeight = 0;

		b2WorldId m_PhysicsWorld = b2_nullWorldId;

        friend class Entity;
		friend class SceneSerializer;
        friend class SceneHierarchyPanel;
    };

}
