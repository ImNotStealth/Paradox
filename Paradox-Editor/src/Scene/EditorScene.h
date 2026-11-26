#pragma once

#include <Paradox/Scene/Scene.h>

#include <glm/glm.hpp>
#include <entt/entt.hpp>

namespace Paradox
{
	class EditorCamera;
	class TransformComponent;

	enum class SceneState
	{
		Stopped,
		Playing
	};

	class EditorScene : public Scene
	{
	public:
		void UpdateEditor(const EditorCamera& editorCamera);

	private:
		glm::mat4 GetBillboardTransform(const EditorCamera& editorCamera, const TransformComponent& transformComp);

	private:
		friend class Entity;
	};
}