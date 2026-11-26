#pragma once

#include "Paradox/Core/Base.h"

#include <glm/glm.hpp>

namespace Paradox
{
	enum class CameraProjectionType
	{
		Perspective,
		Orthographic
	};

	class PARADOX_API Camera
	{
	public:
		Camera() = default;
		Camera(float fov, float aspectRatio, float nearClip, float farClip);
		virtual ~Camera() = default;

		void UpdateProjection();

		inline float GetViewportWidth() const { return m_ViewportWidth; }
		inline float GetViewportHeight() const { return m_ViewportHeight; }
		inline const glm::mat4& GetProjection() const { return m_Projection; }
		inline float GetFieldOfView() const { return m_FOV; }
		inline float GetNearClip() const { return m_NearClip; }
		inline float GetFarClip() const { return m_FarClip; }
		inline float GetOrthographicSize() const { return m_OrthographicSize; }
		inline CameraProjectionType GetProjectionType() const { return m_ProjectionType; }

		inline void SetFieldOfView(float fovDeg) { m_FOV = fovDeg; }
		inline void SetNearClip(float nearClip) { m_NearClip = nearClip; }
		inline void SetFarClip(float farClip) { m_FarClip = farClip; }
		inline void SetOrthographicSize(float size) { m_OrthographicSize = size; }
		inline void SetProjectionType(CameraProjectionType type) { m_ProjectionType = type; }
		void SetViewportSize(float width, float height);

		static std::string ProjectionTypeToString(CameraProjectionType type);
		static CameraProjectionType StringToProjectionType(const std::string& str);

	protected:
		glm::mat4 m_Projection = glm::mat4(1.f);

		CameraProjectionType m_ProjectionType = CameraProjectionType::Perspective;

		float m_ViewportWidth = 1280.f, m_ViewportHeight = 720.f;
		float m_FOV = 45.0f, m_AspectRatio = (16.f / 9.f), m_NearClip = 0.1f, m_FarClip = 1000.0f;
		float m_OrthographicSize = 10.f;
	};
}