#include "EditorScene.h"

#include "EditorApp.h"
#include "Utils/EditorIcons.h"
#include "Utils/EditorCamera.h"

#include <Paradox.h>

namespace Paradox
{
	void EditorScene::UpdateEditor(const EditorCamera& editorCamera)
	{
		PX_PROFILE_FUNCTION();
		Renderer2D::Begin(editorCamera.GetViewProjection());

		{
			auto view = m_Registry.view<TransformComponent, SpriteComponent>();
			for (auto entity : view)
			{
				TransformComponent& transform = view.get<TransformComponent>(entity);
				SpriteComponent& sprite = view.get<SpriteComponent>(entity);

				Reference<Texture2D> texture = sprite.texture != UUID::EMPTY ? AssetManager::Get()->GetAsset<Texture2D>(sprite.texture) : nullptr;
				if (texture)
					Renderer2D::DrawQuad(transform.GetTransform(), texture, sprite.color, sprite.tilingFactor, sprite.uv0, sprite.uv1);
				else
					Renderer2D::DrawQuad(transform.GetTransform(), sprite.color);
			}
		}

		{
			auto view = m_Registry.view<TransformComponent, CameraComponent>();
			for (auto entity : view)
			{
				TransformComponent& transform = view.get<TransformComponent>(entity);

				EditorApp& app = (EditorApp&)Application::Get();
				Renderer2D::DrawQuad(GetBillboardTransform(editorCamera, transform), app.GetEditorAssetManager()->GetAsset<Texture2D>(UUID(ICON_CAMERA)));
			}
		}

		Renderer2D::End();
	}

	glm::mat4 EditorScene::GetBillboardTransform(const EditorCamera& editorCamera, const TransformComponent& transformComp)
	{
		PX_PROFILE_FUNCTION();
		const glm::mat4 view = editorCamera.GetView();
		const glm::vec3 camRight = { view[0][0], view[1][0], view[2][0] };
		const glm::vec3 camUp = { view[0][1], view[1][1], view[2][1] };
		const glm::vec3 camForward = { view[0][2], view[1][2], view[2][2] };

		glm::mat4 transform = glm::mat4(1.f);
		transform[0] = glm::vec4(camRight, 0.f);
		transform[1] = glm::vec4(camUp, 0.f);
		transform[2] = glm::vec4(camForward, 0.f);
		transform[3] = glm::vec4(transformComp.position, 1.f);
		return transform;
	}
}
