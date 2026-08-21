#pragma once

#include "Paradox/Core/Base.h"
#include "Paradox/Scene/UUID.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Paradox
{
	struct IDComponent
	{
		UUID id;
	};

	struct TransformComponent
	{
		glm::vec3 position = { 0.0f, 0.0f, 0.0f };
		glm::vec3 rotation = { 0.0f, 0.0f, 0.0f };
		glm::vec3 scale = { 1.0f, 1.0f, 1.0f };

		glm::mat4 GetTransform() const
		{
			glm::mat4 rotationMat = glm::toMat4(glm::quat(rotation));
			return glm::translate(glm::mat4(1.0f), position) * rotationMat * glm::scale(glm::mat4(1.0f), scale);
		}
	};

	struct NameComponent
	{
		std::string name;
	};

	struct SpriteComponent
	{
		Shared<class Texture2D> texture = nullptr;
		glm::vec4 color = glm::vec4(1.f);
		float tilingFactor = 1.f;
		glm::vec2 uv0 = { 0.f, 0.f }, uv1 = { 1.f, 1.f };
	};
}