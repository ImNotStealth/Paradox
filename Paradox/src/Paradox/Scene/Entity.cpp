#include "pxpch.h"
#include "Entity.h"

namespace Paradox
{
	Entity::Entity(entt::entity handle, Scene* scene)
		: m_Handle(handle), m_Scene(scene) {}

	void Entity::DestroySelf()
	{
		m_Scene->m_Registry.destroy(m_Handle);
	}

	bool Entity::IsValid() const
	{
		return m_Handle != entt::null && m_Scene->m_Registry.valid(m_Handle);
	}
}