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
		glm::vec3 forward = glm::vec3(sin(-m_Rotation.y), 0.f, -cos(-m_Rotation.y));
		glm::vec3 right = glm::vec3(cos(-m_Rotation.y), 0.f, sin(-m_Rotation.y));

		glm::vec3 direction = glm::vec3(0.f);

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

		m_Position += direction * deltaTime;

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