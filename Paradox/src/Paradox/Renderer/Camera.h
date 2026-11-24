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

		void SetViewportSize(float width, float height) { m_ViewportWidth = width; m_ViewportHeight = height; UpdateViewportSize(); }
		float GetViewportWidth() const { return m_ViewportWidth; }
		float GetViewportHeight() const { return m_ViewportHeight; }

		inline const glm::mat4& GetProjection() const { return m_Projection; }

		void UpdateViewportSize();

	protected:
		glm::mat4 m_Projection = glm::mat4(1.f);

		float m_ViewportWidth = 1280.f, m_ViewportHeight = 720.f;
		float m_FOV = 45.0f, m_AspectRatio = (16.f / 9.f), m_NearClip = 0.1f, m_FarClip = 1000.0f;
	};
}