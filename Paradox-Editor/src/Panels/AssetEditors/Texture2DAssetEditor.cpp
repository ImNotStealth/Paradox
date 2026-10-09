#include "Texture2DAssetEditor.h"

#include "Project/Project.h"
#include "EditorApp.h"

#include <Paradox/ImGui/ImGuiUtils.h>
#include <Paradox/Assets/Metadata/Texture2DMetadata.h>
#include <Paradox.h>

namespace Paradox
{
	Texture2DAssetEditor::Texture2DAssetEditor(Shared<AssetMetadata> meta)
		: AssetEditorPanel(meta->GetUUID().ToString(), meta), m_AppRef((EditorApp&)Application::Get())
	{
		m_Texture = AssetManager::Get()->GetAsset<Texture2D>(m_Metadata->GetUUID());

		uint32_t width = m_Texture->GetWidth();
		uint32_t divisor = m_Texture->GetHeight();
		while (divisor != 0)
		{
			uint32_t remainder = width % divisor;
			width = divisor;
			divisor = remainder;
		}
		uint32_t gcd = width;
		m_TextureRatio = { m_Texture->GetWidth() / gcd, m_Texture->GetHeight() / gcd };
		m_TextureRatiof = (float)m_Texture->GetWidth() / (float)m_Texture->GetHeight();
	}

	void Texture2DAssetEditor::OnImGuiRender(bool* opened, float deltaTime)
	{
		PX_PROFILE_FUNCTION();

		std::string fileName = m_Metadata->GetMetaPath().filename().replace_extension().string();
		ImGui::SetNextWindowDockID(m_AppRef.GetViewportPanel()->GetDockID(), ImGuiCond_FirstUseEver);
		if (!ImGui::Begin(fileName.c_str(), opened))
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
		ImGui::Checkbox("Draw Grid", &m_DrawGrid);
		ImGui::SameLine();

		ImGui::PushItemWidth(250.f);
		ImGui::SliderFloat("Zoom", &m_Zoom, m_ZoomMin, m_ZoomMax);
		ImGui::PopItemWidth();

		if (ImGui::BeginChild("##props", ImVec2(-300, 0), ImGuiChildFlags_ResizeX | ImGuiChildFlags_Borders | ImGuiChildFlags_NavFlattened))
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

			if (m_ResetView)
				m_ViewOffset = ImVec2((canvas_size.x * 0.5f / m_Zoom) - 0.5f, (canvas_size.y * 0.5f / m_Zoom) - 0.5f); // Add half a pixel padding
			m_ResetView = false;

			// Handle inputs
			if (ImGui::SetItemKeyOwner(ImGuiKey_MouseWheelY))
				if (io.MouseWheel != 0.0f)
					m_Zoom = std::clamp<float>(m_Zoom * (1.0f + io.MouseWheel * 0.10f), m_ZoomMin, m_ZoomMax);
			if (ImGui::IsItemActive() && ImGui::IsMouseDragging(0))
			{
				m_ViewOffset.x -= io.MouseDelta.x / m_Zoom;
				m_ViewOffset.y -= io.MouseDelta.y / m_Zoom;
			}

			// Display image
			ImVec2 image_min, image_max;
			image_min.x = (float)(int)((canvas_min.x - (m_ViewOffset.x * m_Zoom)) + (canvas_size.x * 0.5f));
			image_min.y = (float)(int)((canvas_min.y - (m_ViewOffset.y * m_Zoom)) + (canvas_size.y * 0.5f));
			image_max.x = (float)(int)(image_min.x + image_w * m_Zoom);
			image_max.y = (float)(int)(image_min.y + image_h * m_Zoom);
			//draw_list->AddRect(ImVec2(canvas_min.x - 1.0f, canvas_min.y - 1.0f), ImVec2(canvas_max.x + 1.0f, canvas_max.y + 1.0f), ImGui::GetColorU32(ImGuiCol_TableBorderLight));
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
			if (m_DrawGrid && m_Zoom > 6.0f)
			{
				const ImU32 gridColor = IM_COL32(255, 255, 255, 100);
				const float step = (float)m_Zoom;
				for (int px = (int)((canvas_min.x - image_min.x) / step); px <= (int)((canvas_max.x - image_min.x) / step); px++)
					draw_list->AddLineV(image_min.x + px * step, canvas_min.y, canvas_max.y, gridColor, 1.0f);
				for (int py = (int)((canvas_min.y - image_min.y) / step); py <= (int)((canvas_max.y - image_min.y) / step); py++)
					draw_list->AddLineH(canvas_min.x, canvas_max.x, image_min.y + py * step, gridColor, 1.0f);
			}
			draw_list->PopClipRect();

			ImGui::EndChild();
		}

		ImGui::SameLine();
		ImGui::BeginGroup();

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1], ImGui::GetFontSize() * 1.2f);
		ImGui::TextUnformatted(fileName.c_str());
		ImGui::PopFont();
		ImGui::Separator();

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::TextUnformatted("Details");
		ImGui::PopFont();
		ImGui::Text("UUID: %s", m_Metadata->GetUUID().ToString().c_str());
		std::filesystem::path relativePath = std::filesystem::relative(m_Metadata->GetMetaPath(), Project::GetActive()->GetProperties().path);
		ImGui::Text("Meta Path: %s", relativePath.string().c_str());
		ImGui::Text("Texture Dimensions: %dx%d", m_Texture->GetWidth(), m_Texture->GetHeight());
		ImGui::Text("Displayed Dimensions: %dx%d", (int)(m_Texture->GetWidth() * m_Zoom), (int)(m_Texture->GetHeight() * m_Zoom));
		ImGui::Text("Pixel Ratio: %d:%d (%f)", m_TextureRatio.x, m_TextureRatio.y, m_TextureRatiof);

		ImGui::Dummy({ 0.f, 10.f });
		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		ImGui::TextUnformatted("Properties");
		ImGui::PopFont();

		ImGui::PushItemWidth(300.f);
		Shared<Texture2DMetadata> textureMeta = std::static_pointer_cast<Texture2DMetadata>(m_Metadata);
		{
			const char* filterModes[] = { "Nearest", "Linear" };
			int textureFilter = (int)textureMeta->GetFilter();
			if (ImGui::Combo("Filter", &textureFilter, filterModes, IM_ARRAYSIZE(filterModes)))
			{
				textureMeta->SetFilter((TextureFilter)textureFilter);
				AssetManager::Get()->InvalidateAsset(textureMeta->GetUUID());
				m_ReloadAsset = true;
			}
			ImGuiUtils::HelpMarker("Default: Nearest", true);
		}

		{
			const char* wrapModes[] = { "Repeat", "Mirrored Repeat", "Clamp to Border (Incompatible with PS Vita)", "Clamp to Edge" };
			int textureWrap = (int)textureMeta->GetWrap();
			if (ImGui::Combo("Wrap", &textureWrap, wrapModes, IM_ARRAYSIZE(wrapModes)))
			{
				textureMeta->SetWrap((TextureWrap)textureWrap);
				AssetManager::Get()->InvalidateAsset(textureMeta->GetUUID());
				m_ReloadAsset = true;
			}
			ImGuiUtils::HelpMarker("Default: Repeat", true);
		}

		{
			const char* formats[] = { "SRGBA", "RGBA", "BGRA" };
			int textureFormat = (int)textureMeta->GetFormat();
			if (ImGui::Combo("Format", &textureFormat, formats, IM_ARRAYSIZE(formats)))
			{
				textureMeta->SetFormat((ImageFormat)textureFormat);
				AssetManager::Get()->InvalidateAsset(textureMeta->GetUUID());
				m_ReloadAsset = true;
			}
			ImGuiUtils::HelpMarker("Default: RGBA", true);
		}

		{
			bool anisotropicFiltering = textureMeta->GetAnisotropicFiltering();
			if (ImGui::Checkbox("Anisotropic Filtering", &anisotropicFiltering))
			{
				textureMeta->SetAnisotropicFiltering(anisotropicFiltering);
				AssetManager::Get()->InvalidateAsset(textureMeta->GetUUID());
				m_ReloadAsset = true;
			}
			ImGuiUtils::HelpMarker("Default: True", true);
		}

		ImGui::PopItemWidth();
		ImGui::EndGroup();
		ImGui::End();
	}
}