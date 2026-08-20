#include "pxpch.h"
#include "Scene.h"

#include "Paradox/Scene/Entity.h"
#include "Paradox/Scene/Components.h"
#include "Paradox/Renderer/Renderer2D.h"

namespace Paradox
{
	Scene::Scene(const std::string& name)
		: m_Name(name), m_Registry(entt::registry())
	{}

	void Scene::Update(const glm::mat4& projection, float deltaTime)
	{
		Renderer2D::Begin(projection);

		auto view = m_Registry.view<TransformComponent, SpriteComponent>();
		for (auto entity : view)
		{
			TransformComponent& transform = view.get<TransformComponent>(entity);
			SpriteComponent& sprite = view.get<SpriteComponent>(entity);
			Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
		}

		Renderer2D::End();
	}

	Entity Scene::CreateEntity(const std::string& name)
	{
		Entity entity = Entity(m_Registry.create(), this);
		entity.AddComponent<IDComponent>();
		entity.AddComponent<TransformComponent>();
		entity.AddComponent<NameComponent>(name);
		return entity;
	}

	void Scene::DestroyEntity(Entity entity)
	{
		m_Registry.destroy(entity);
	}
}