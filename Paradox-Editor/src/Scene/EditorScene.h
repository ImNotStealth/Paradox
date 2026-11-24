#pragma once

#include <Paradox/Scene/Scene.h>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace Paradox
{
	enum class SceneState
	{
		Stopped,
		Playing
	};

	class EditorScene : public Scene
	{
	public:
		void UpdateEditor(const glm::mat4& viewProj);

	private:
		friend class Entity;
	};
}