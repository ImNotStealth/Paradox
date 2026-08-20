	#pragma once

	#include "Paradox/Core/Base.h"

	#include <glm/glm.hpp>
	#include <entt/entt.hpp>

	namespace Paradox
	{
		class Entity;

		class PARADOX_API Scene
		{
		public:
			Scene(const std::string& name = "Scene");

			void Update(const glm::mat4& projection, float deltaTime);

			Entity CreateEntity(const std::string& name = "Entity");
			void DestroyEntity(Entity entity);

			template<typename... T>
			auto GetEntitiesWith()
			{
				return EntityView<T...>(m_Registry, this);
			}

			const std::string& GetName() { return m_Name; }

		private:
			std::string m_Name;
			entt::registry m_Registry;
			friend class Entity;
		};
	}