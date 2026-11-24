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
	{}

	void Camera::UpdateViewportSize()
	{
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);

		// Flipping Y because Vulkan and OpenGL can't get along apparently :shrug:
		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::Vulkan)
			m_Projection[1][1] *= -1.f;
	}
}