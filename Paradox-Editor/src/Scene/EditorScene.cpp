#include "EditorScene.h"

#include <Paradox.h>

namespace Paradox
{
	void EditorScene::UpdateEditor(const glm::mat4& viewProj)
	{
		Renderer2D::Begin(viewProj);

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

		Renderer2D::End();
	}
}
