#include "pxpch.h"
#include "Camera.h"

#include "Paradox/Core/Input.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Paradox
{
	Camera::Camera(float aspectRatio, float zoom)
		: Camera(-aspectRatio * zoom, aspectRatio * zoom, -zoom, zoom) {}

	Camera::Camera(float left, float right, float bottom, float top)
		: m_Projection(glm::ortho(left, right, bottom, top))
	{
		m_ViewProjection = m_Projection * m_View;
	}

	void Camera::Update()
	{
		if (Input::IsKeyPressed(Keyboard::W))
			m_Position.y += 0.001f;
		else if (Input::IsKeyPressed(Keyboard::S))
			m_Position.y -= 0.001f;

		if (Input::IsKeyPressed(Keyboard::A))
			m_Position.x -= 0.001f;
		else if (Input::IsKeyPressed(Keyboard::D))
			m_Position.x += 0.001f;

		m_Rotation += 0.001f;

		RecalculateView();
	}

	void Paradox::Camera::RecalculateView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.f), m_Position) * glm::rotate(glm::mat4(1.f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}
}