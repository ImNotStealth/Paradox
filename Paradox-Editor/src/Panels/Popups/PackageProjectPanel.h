#pragma once

#include "Panels/Panel.h"

#include <future>

namespace Paradox
{
	class PackageProjectPanel : public Panel
	{
	public:
		PackageProjectPanel()
			: Panel("Package Project")
		{}

		void OnImGuiRender(bool* opened, float deltaTime) override;

	private:
		void Package();
		bool Compile();
		bool CopyAssets();

	private:
		std::atomic<uint8_t> m_CaughtError = 0;
		bool m_IncludeImGui = false;
		std::filesystem::path m_TargetBuildPath, m_BinFilePath;
		std::future<void> m_CompileFuture;
	};
}