#include "pxpch.h"
#include "Camera.h"

#include "Paradox/Core/Input.h"
#include "Paradox/Renderer/GraphicsContext.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Paradox
{
	Camera::Camera(float fov, float aspectRatio, float nearClip, float farClip)
		: m_FOV(fov), m_AspectRatio(aspectRatio), m_NearClip(nearClip), m_FarClip(farClip)
	{
		RecalculateView();
	}

	void Camera::Update(float deltaTime)
	{
		if (Input::IsKeyPressed(Keyboard::W))
			m_Position.z -= 1.f * deltaTime;
		else if (Input::IsKeyPressed(Keyboard::S))
			m_Position.z += 1.f * deltaTime;

		if (Input::IsKeyPressed(Keyboard::A))
			m_Position.x -= 1.f * deltaTime;
		else if (Input::IsKeyPressed(Keyboard::D))
			m_Position.x += 1.f * deltaTime;

		if (Input::IsKeyPressed(Keyboard::Q))
			m_Position.y -= 1.f * deltaTime;
		else if (Input::IsKeyPressed(Keyboard::E))
			m_Position.y += 1.f * deltaTime;

		if (Input::IsKeyPressed(Keyboard::Left))
			m_Rotation.y += 1.f * deltaTime;
		else if (Input::IsKeyPressed(Keyboard::Right))
			m_Rotation.y -= 1.f * deltaTime;

		if (Input::IsKeyPressed(Keyboard::Up))
			m_Rotation.x += 1.f * deltaTime;
		else if (Input::IsKeyPressed(Keyboard::Down))
			m_Rotation.x -= 1.f * deltaTime;

		RecalculateView();
	}

	void Camera::UpdateViewportSize()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);

		// Flipping Y because Vulkan and OpenGL can't get along apparently :shrug:
		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::Vulkan)
			m_Projection[1][1] *= -1.f;
	}

	void Camera::RecalculateView()
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.f), m_Position) * glm::toMat4(glm::quat(m_Rotation));

		m_View = glm::inverse(transform);
		m_ViewProjection = m_Projection * m_View;
	}
}