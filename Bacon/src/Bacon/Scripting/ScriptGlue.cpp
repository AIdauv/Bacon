#include "bcpch.h"
#include "ScriptGlue.h"

#include "ScriptEngine.h"

#include "Bacon/Core/UUID.h"
#include "Bacon/Core/KeyCodes.h"
#include "Bacon/Core/Input.h"

#include "Bacon/Scene/Scene.h"
#include "Bacon/Scene/Entity.h"

#include "mono/metadata/object.h"
#include "mono/metadata/reflection.h"

#include "box2d/id.h"

#include <glm/glm.hpp>

namespace Bacon {

	static std::unordered_map<MonoType*, std::function<bool(Entity)>> s_EntityHasComponentFuncs;

#define BC_ADD_INTERNAL_CALL(Name) mono_add_internal_call("Bacon.InternalCalls::" #Name, Name)

	static void NativeLog(MonoString* string, int parameter)
	{
		char* cStr = mono_string_to_utf8(string);
		std::string str(cStr);
		mono_free(cStr);
		std::cout << str << ", " << parameter << std::endl;
	}

	static void NativeLog_Vector(glm::vec3* parameter, glm::vec3* outResult)
	{
		BC_CORE_WARN("Value: {0}", *parameter);
		*outResult = glm::normalize(*parameter);
	}

	static float NativeLog_VectorDot(glm::vec3* parameter)
	{
		BC_CORE_WARN("Value: {0}", *parameter);
		return glm::dot(*parameter, *parameter);
	}

	static bool Entity_HasComponent(UUID entityID, MonoReflectionType* componentType)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		BC_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		BC_CORE_ASSERT(entity);

		MonoType* managedType = mono_reflection_type_get_type(componentType);
		BC_CORE_ASSERT(s_EntityHasComponentFuncs.find(managedType) != s_EntityHasComponentFuncs.end());
		return s_EntityHasComponentFuncs.at(managedType)(entity);
	}

	static void TransformComponent_GetTranslation(UUID entityID, glm::vec3* outTranslation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		BC_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		BC_CORE_ASSERT(entity);

		*outTranslation = entity.GetComponent<TransformComponent>().Translation;
	}

	static void TransformComponent_SetTranslation(UUID entityID, glm::vec3* translation)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		BC_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		BC_CORE_ASSERT(entity);

		entity.GetComponent<TransformComponent>().Translation = *translation;
	}

	static void Rigidbody2DComponent_ApplyLinearImpulse(UUID entityID, glm::vec2* impulse, glm::vec2* point, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		BC_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		BC_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2BodyId body = rb2d.RuntimeBody;
		b2Body_ApplyLinearImpulse(body, b2Vec2(impulse->x, impulse->y), b2Vec2(point->x, point->y), wake);
	}

	static void Rigidbody2DComponent_ApplyLinearImpulseToCenter(UUID entityID, glm::vec2* impulse, bool wake)
	{
		Scene* scene = ScriptEngine::GetSceneContext();
		BC_CORE_ASSERT(scene);
		Entity entity = scene->GetEntityByUUID(entityID);
		BC_CORE_ASSERT(entity);

		auto& rb2d = entity.GetComponent<Rigidbody2DComponent>();
		b2BodyId body = rb2d.RuntimeBody;
		b2Body_ApplyLinearImpulseToCenter(body, b2Vec2(impulse->x, impulse->y), wake);
	}

	static bool Input_IsKeyDown(KeyCode keycode)
	{
		return Input::IsKeyPressed(keycode);
	}

	template<typename... Component>
	static void RegisterComponent()
	{
		([]()
			{
				std::string_view typeName = typeid(Component).name();
				size_t pos = typeName.find_last_of(':');
				std::string_view structName = typeName.substr(pos + 1);
				std::string managedTypename = fmt::format("Bacon.{}", structName);

				MonoType* managedType = mono_reflection_type_from_name(managedTypename.data(), ScriptEngine::GetCoreAssemblyImage());
				if (!managedType)
				{
					BC_CORE_ERROR("Could not find component type {}", managedTypename);
					return;
				}
				s_EntityHasComponentFuncs[managedType] = [](Entity entity) { return entity.HasComponent<Component>(); };
			}(), ...);
	}

	template<typename... Component>
	static void RegisterComponent(ComponentGroup<Component...>)
	{
		RegisterComponent<Component...>();
	}

	void ScriptGlue::RegisterComponents()
	{
		RegisterComponent(AllComponents{});
	}

	void ScriptGlue::RegisterFunctions()
	{
		BC_ADD_INTERNAL_CALL(NativeLog);
		BC_ADD_INTERNAL_CALL(NativeLog_Vector);
		BC_ADD_INTERNAL_CALL(NativeLog_VectorDot);

		BC_ADD_INTERNAL_CALL(Entity_HasComponent);
		BC_ADD_INTERNAL_CALL(TransformComponent_GetTranslation);
		BC_ADD_INTERNAL_CALL(TransformComponent_SetTranslation);

		BC_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulse);
		BC_ADD_INTERNAL_CALL(Rigidbody2DComponent_ApplyLinearImpulseToCenter);

		BC_ADD_INTERNAL_CALL(Input_IsKeyDown);
	}

}
