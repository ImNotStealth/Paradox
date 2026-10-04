#pragma once

#include <Paradox/Renderer/Camera.h>

namespace Paradox
{
	class EditorCamera : public Camera
	{
	public:
		EditorCamera() = default;
		EditorCamera(float fov, float aspectRatio, float nearClip, float farClip);

		void Update(float deltaTime) override;
		void UpdateInput(float deltaTime);
		void ResetDelta();

		float& GetSpeed() { return m_Speed; }

	private:
		glm::vec2 m_MousePos = {}, m_LastMousePos = {};
		float m_Speed = 10.f;
	};
}