#pragma once

#include "Paradox/Core/Base.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class PARADOX_API Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float nearClip, float farClip);
		virtual ~Camera() = default;
		
		virtual void Update(float deltaTime);
		void RecalculateView();

		void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateViewportSize(); }

		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetProjection() const { return m_Projection; }

		glm::vec3& GetPosition() { return m_Position; }
		glm::vec3& GetRotation() { return m_Rotation; }

	protected:
		void UpdateViewportSize();

	protected:
		glm::mat4 m_Projection = glm::mat4(1.f);
		glm::mat4 m_View = glm::mat4(1.f);
		glm::mat4 m_ViewProjection = glm::mat4(1.f);

		glm::vec3 m_Position = glm::vec3(0.f, 0.f, 2.f);
		glm::vec3 m_Rotation = glm::vec3(0.f);

		float m_ViewportWidth = 1280.f, m_ViewportHeight = 720.f;
		float m_FOV = 45.0f, m_AspectRatio = (16.f / 9.f), m_NearClip = 0.1f, m_FarClip = 1000.0f;
	};
}