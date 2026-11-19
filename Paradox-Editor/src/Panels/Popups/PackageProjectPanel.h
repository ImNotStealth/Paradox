#pragma once

#include "Panels/Panel.h"

#include <future>

namespace Paradox
{
	class PackageProjectPanel : public Panel
	{
	public:
		PackageProjectPanel();

		void OnImGuiRender(bool* opened, float deltaTime) override;

	private:
		void Package();
		bool Compile();
		bool CopyAssets();

	private:
		class EditorApp& m_AppRef;
		std::atomic<uint8_t> m_CaughtError = 0;
		std::filesystem::path m_TargetBuildPath, m_BinFilePath;
		std::future<void> m_CompileFuture;
	};
}