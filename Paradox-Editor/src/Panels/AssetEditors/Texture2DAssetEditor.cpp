#include "Texture2DAssetEditor.h"

#include <Paradox/ImGui/ImGuiUtils.h>
#include <Paradox/Assets/Metadata/Texture2DMetadata.h>
#include <Paradox.h>

namespace Paradox
{
	Texture2DAssetEditor::Texture2DAssetEditor(Shared<AssetMetadata> meta)
		: AssetEditorPanel(meta->GetUUID().ToString(), meta)
	{
		m_Texture = AssetManager::Get()->GetAsset<Texture2D>(m_Metadata->GetUUID());
	}

	void Texture2DAssetEditor::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();

		if (!ImGui::Begin(m_Metadata->GetMetaPath().filename().string().c_str(), opened))
		{
			ImGui::End();
			return;
		}

		if (!m_Texture || m_ReloadAsset)
		{
			Reference<Texture2D> fetched = AssetManager::Get()->GetAsset<Texture2D>(m_Metadata->GetUUID());
			if (fetched && fetched.Get() != m_Texture.Get())
			{
				m_Texture = fetched;
				m_ReloadAsset = false;
			}
		}

		if (ImGui::Button("Save"))
			m_Metadata->Serialize();
		ImGui::SameLine();
		ImGui::Checkbox("Draw Grid", &GridEnabled);
		ImGui::SameLine();
		ImGui::SliderFloat("Zoom", &m_Zoom, m_ZoomMin, m_ZoomMax);

		if (ImGui::BeginChild("##props", ImVec2(300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
		{
			uint32_t image_w = m_Texture->GetWidth();
			uint32_t image_h = m_Texture->GetHeight();

			ImVec2 canvas_size = ImGui::GetContentRegionAvail();
			ImVec2 canvas_min_size = ImGui::IsWindowAppearing() ? ImVec2(3.0f * image_w, 4.0f * image_h) : ImVec2(1.0f, 1.0f);
			canvas_size = ImVec2(std::max(canvas_size.x, canvas_min_size.x), std::max(canvas_size.y, canvas_min_size.y));

			ImGuiIO& io = ImGui::GetIO();
			ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();
			ImDrawList* draw_list = ImGui::GetWindowDrawList();
			IM_ASSERT(canvas_size.x >= 0.0f && canvas_size.y >= 0.0f);

			// Layout canvas
			ImGui::InvisibleButton("##Canvas", canvas_size);
			ImVec2 canvas_min = ImGui::GetItemRectMin();
			ImVec2 canvas_max = ImGui::GetItemRectMax();

			if (ViewReset)
				ViewOffset = ImVec2((canvas_size.x * 0.5f / m_Zoom) - 0.5f, (canvas_size.y * 0.5f / m_Zoom) - 0.5f); // Add half a pixel padding
			ViewReset = false;

			// Handle inputs
			if (ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY))
				if (io.MouseWheel != 0.0f)
					m_Zoom = std::clamp<float>(m_Zoom * (1.0f + io.MouseWheel * 0.10f), m_ZoomMin, m_ZoomMax);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				ViewOffset.x -= io.MouseDelta.x / m_Zoom;
				ViewOffset.y -= io.MouseDelta.y / m_Zoom;
			}

			// Display image
			ImVec2 image_min, image_max;
			image_min.x = (float)(int)((canvas_min.x - (ViewOffset.x * m_Zoom)) + (canvas_size.x * 0.5f));
			image_min.y = (float)(int)((canvas_min.y - (ViewOffset.y * m_Zoom)) + (canvas_size.y * 0.5f));
			image_max.x = (float)(int)(image_min.x + image_w * m_Zoom);
			image_max.y = (float)(int)(image_min.y + image_h * m_Zoom);
			draw_list->AddRect(ImVec2(canvas_min.x - 1.0f, canvas_min.y - 1.0f), ImVec2(canvas_max.x + 1.0f, canvas_max.y + 1.0f), ImGui::GetColorU32(ImGuiCol_TableBorderLight));
			draw_list->PushClipRect(canvas_min, canvas_max, true);
			draw_list->AddRectFilled(image_min, image_max, IM_COL32(204, 204, 204, 255));
			int row = 0;
			float tileSize = 16.f;
			for (float y = image_min.y; y < image_max.y; y += tileSize, ++row)
			{
				float xStart = image_min.x + ((row & 1) ? tileSize : 0.f);
				for (float x = xStart; x < image_max.x; x += tileSize * 2.f)
				{
					ImVec2 p0 = { x, y };
					ImVec2 p1 = { std::min(x + tileSize, image_max.x), std::min(y + tileSize, image_max.y) };
					draw_list->AddRectFilled(p0, p1, IM_COL32(153, 153, 153, 255));
				}
			}
			draw_list->AddCallback(m_Texture->GetProperties().minFilter == TextureFilter::Nearest ?
				ImGui::GetPlatformIO().DrawCallback_SetSamplerNearest : ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear);
			draw_list->AddImage(ImGuiUtils::GetImageID(m_Texture->GetImage()), image_min, image_max);
			draw_list->AddCallback(ImGui::GetPlatformIO().DrawCallback_SetSamplerLinear);

			// Display grid lines for visible pixels
			if (GridEnabled && m_Zoom > 6.0f)
			{
				const float step = (float)m_Zoom;
				for (int px = (int)((canvas_min.x - image_min.x) / step); px <= (int)((canvas_max.x - image_min.x) / step); px++)
					draw_list->AddLineV(image_min.x + px * step, canvas_min.y, canvas_max.y, GridColor, 1.0f);
				for (int py = (int)((canvas_min.y - image_min.y) / step); py <= (int)((canvas_max.y - image_min.y) / step); py++)
					draw_list->AddLineH(canvas_min.x, canvas_max.x, image_min.y + py * step, GridColor, 1.0f);
			}
			draw_list->PopClipRect();

			ImGui::EndChild();
		}

		ImGui::SameLine();

		ImGui::BeginGroup();
		ImGui::Text("Texture Dimensions: %dx%d", m_Texture->GetWidth(), m_Texture->GetHeight());
		ImGui::Text("Displayed Dimensions: %dx%d", m_Texture->GetWidth() * m_Zoom, m_Texture->GetHeight() * m_Zoom);

		if (ImGui::Button("Toggle Filter"))
		{
			Shared<Texture2DMetadata> textureMeta = std::static_pointer_cast<Texture2DMetadata>(m_Metadata);
			textureMeta->SetFilter(textureMeta->GetFilter() == TextureFilter::Linear ? TextureFilter::Nearest : TextureFilter::Linear);
			AssetManager::Get()->InvalidateAsset(textureMeta->GetUUID());
			m_ReloadAsset = true;
		}
		ImGui::EndGroup();
		ImGui::End();
	}
}