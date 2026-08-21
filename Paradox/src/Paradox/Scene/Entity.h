#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Scene/Scene.h"

#include <entt/entt.hpp>

namespace Paradox
{
	class PARADOX_API Entity
	{
	public:
		Entity() = default;

		void DestroySelf();
		bool IsValid() const;

		template<typename T, typename... Args>
		T& AddComponent(Args&&... args)
		{
			PX_CORE_ASSERT(!HasComponents<T>(), "An entity may not have duplicate components.");
			return m_Scene->m_Registry.emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T& GetComponent()
		{
			PX_CORE_ASSERT(HasComponents<T>(), "Entity does not have component.");
			return m_Scene->m_Registry.get<T>(m_Handle);
		}

		template<typename T, typename... Args>
		T& GetOrAddComponent(Args&&... args)
		{
			PX_CORE_ASSERT(IsValid());
			return m_Scene->m_Registry.get_or_emplace<T>(m_Handle, std::forward<Args>(args)...);
		}

		template<typename T>
		T* TryGetComponent()
		{
			PX_CORE_ASSERT(IsValid());
			return m_Scene->m_Registry.try_get<T>(m_Handle);
		}

		template<typename T>
		void RemoveComponents()
		{
			PX_CORE_ASSERT(HasComponents<T>(), "Entity does not have component.");
			m_Scene->m_Registry.remove<T>(m_Handle);
		}

		template<typename... T>
		bool HasComponents() const
		{
			PX_CORE_ASSERT(IsValid());
			return m_Scene->m_Registry.all_of<T...>(m_Handle);
		}

		operator bool() const { return m_Handle != entt::null; }
		operator entt::entity() const { return m_Handle; }
		operator uint32_t() const { return (uint32_t)m_Handle; }

	private:
		Entity(entt::entity handle, Scene* scene);

	private:
		entt::entity m_Handle = entt::null;
		Scene* m_Scene = nullptr;

		friend class Scene;
		template<typename...> friend class EntityView;
	};

	template<typename... T>
	class EntityView
	{
	public:
		EntityView(entt::registry& registry, Scene* scene)
			: m_View(registry.view<T...>()), m_Scene(scene) {}

		class Iterator
		{
		public:
			Iterator(typename entt::view<entt::get_t<T...>>::iterator it, Scene* scene)
				: m_It(it), m_Scene(scene) {
			}

			Iterator& operator++() { ++m_It; return *this; }
			bool operator!=(const Iterator& other) const { return m_It != other.m_It; }
			bool operator==(const Iterator& other) const { return m_It == other.m_It; }
			Entity operator*() const { return Entity(*m_It, m_Scene); }

		private:
			typename entt::view<entt::get_t<T...>>::iterator m_It;
			Scene* m_Scene;
		};

		Iterator begin() { return Iterator(m_View.begin(), m_Scene); }
		Iterator end() { return Iterator(m_View.end(), m_Scene); }
	private:
		entt::view<entt::get_t<T...>> m_View;
		Scene* m_Scene;
	};
}