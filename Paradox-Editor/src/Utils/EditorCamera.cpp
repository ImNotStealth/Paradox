#include "EditorCamera.h"

#include <Paradox/Core/Input.h>

namespace Paradox
{
	EditorCamera::EditorCamera(float fov, float aspectRatio, float nearClip, float farClip)
		: Camera(fov, aspectRatio, nearClip, farClip)
	{
		m_MousePos = Input::GetMousePos();
		m_LastMousePos = m_MousePos;
	}

	void EditorCamera::Update(float deltaTime)
	{
		RecalculateView();
	}

	void EditorCamera::UpdateInput(float deltaTime)
	{
		glm::vec3 forward = glm::vec3(sin(-m_Rotation.y), 0.f, -cos(-m_Rotation.y));
		glm::vec3 right = glm::vec3(cos(-m_Rotation.y), 0.f, sin(-m_Rotation.y));
		glm::vec3 direction = glm::vec3(0.f);

		m_MousePos = Input::GetMousePos();
		glm::vec2 mouseDelta = { m_MousePos.x - m_LastMousePos.x, m_MousePos.y - m_LastMousePos.y };

		PX_WARN("Delta: {0}x{1} / {2}", mouseDelta.x, mouseDelta.y, deltaTime);


		if (Input::IsKeyPressed(Keyboard::W))
			direction += forward;
		else if (Input::IsKeyPressed(Keyboard::S))
			direction -= forward;

		if (Input::IsKeyPressed(Keyboard::A))
			direction -= right;
		else if (Input::IsKeyPressed(Keyboard::D))
			direction += right;

		if (Input::IsKeyPressed(Keyboard::Q))
			direction.y -= 1.f;
		else if (Input::IsKeyPressed(Keyboard::E))
			direction.y += 1.f;

		if (glm::length(direction) > 0.f)
			direction = glm::normalize(direction);

		m_Position += direction * 10.f * deltaTime;

		m_Rotation.y -= mouseDelta.x * deltaTime;
		m_Rotation.x -= mouseDelta.y * deltaTime;

		m_LastMousePos = Input::GetMousePos();
	}
}