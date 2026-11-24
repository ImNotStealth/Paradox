#pragma once

#include <Paradox/Renderer/Camera.h>

namespace Paradox
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void RecalculateView();
		void UpdateInput(float deltaTime);
		void ResetDelta();

		inline const glm::mat4& GetViewProjection() const { return m_ViewProjection; }
		inline const glm::mat4& GetView() const { return m_View; }

		inline glm::vec3& GetPosition() { return m_Position; }
		inline glm::vec3& GetRotation() { return m_Rotation; }

		inline void SetPosition(const glm::vec3& pos) { m_Position = pos; }
		inline void SetRotation(const glm::vec3& rot) { m_Rotation = rot; }

		inline float GetSpeed() { return m_Speed; }
		inline void SetSpeed(float speed) { m_Speed = speed; }

	private:
		glm::mat4 m_View = glm::mat4(1.f);
		glm::mat4 m_ViewProjection = glm::mat4(1.f);

		glm::vec3 m_Position = glm::vec3(0.f, 2.f, 5.f);
		glm::vec3 m_Rotation = glm::vec3(0.f);

		glm::vec2 m_MousePos = {}, m_LastMousePos = {};
		float m_Speed = 10.f;
	};
}