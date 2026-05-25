#pragma once

#include "Paradox/Core/Base.h"

#include <glm/glm.hpp>

namespace Paradox
{
	class PARADOX_API Camera
	{
	public:
		Camera(float aspectRatio, float zoom);
		Camera(float left, float right, float bottom, float top);
		virtual ~Camera() = default;
		
		const glm::mat4& GetProjection() const { return m_Projection; }
		const glm::mat4& GetView() const { return m_View; }
		const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

		const glm::vec3& GetPosition() const { return m_Position; }

		//TEMPORARY
		void Update();

	private:
		void RecalculateView();

	protected:
		glm::mat4 m_Projection = glm::mat4(1.f);
		glm::mat4 m_View = glm::mat4(1.f);
		glm::mat4 m_ViewProjection = glm::mat4(1.f);

		glm::vec3 m_Position = glm::vec3(0.f);
		float m_Rotation = 0.f;
	};
}