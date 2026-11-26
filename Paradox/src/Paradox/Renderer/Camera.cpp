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

	void Camera::UpdateProjection()
	{
		switch (m_ProjectionType)
		{
		case CameraProjectionType::Perspective:
			m_Projection = glm::perspective(glm::radians(m_FOV), m_AspectRatio, m_NearClip, m_FarClip);
			break;
		case CameraProjectionType::Orthographic:
		{
			float left = -m_OrthographicSize * m_AspectRatio * 0.5f;
			float right = m_OrthographicSize * m_AspectRatio * 0.5f;
			float bottom = -m_OrthographicSize * 0.5f;
			float top = m_OrthographicSize * 0.5f;
			m_Projection = glm::ortho(left, right, bottom, top, m_NearClip, m_FarClip);
			break;
		}
		}

		// Flipping Y because Vulkan and OpenGL can't get along apparently :shrug:
		if (GraphicsContext::GetGraphicsAPI() == GraphicsAPIType::Vulkan)
			m_Projection[1][1] *= -1.f;
	}

	void Camera::SetViewportSize(float width, float height)
	{
		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_AspectRatio = m_ViewportWidth / m_ViewportHeight;
		UpdateProjection();
	}

	std::string Camera::ProjectionTypeToString(CameraProjectionType type)
	{
		switch (type)
		{
		case CameraProjectionType::Perspective: return "Perspective";
		case CameraProjectionType::Orthographic: return "Orthographic";
		}
		PX_CORE_ASSERT(false, "Unknown CameraProjectionType");
		return "";
	}

	CameraProjectionType Camera::StringToProjectionType(const std::string& str)
	{
		if (str == "Perspective") return CameraProjectionType::Perspective;
		if (str == "Orthographic") return CameraProjectionType::Orthographic;

		PX_CORE_ASSERT(false, "Invalid CameraProjectionType");
		return CameraProjectionType::Perspective;
	}
}