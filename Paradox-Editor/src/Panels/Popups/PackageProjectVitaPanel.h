#pragma once

#include "Panels/Panel.h"

#include <future>

namespace Paradox
{
    class PackageProjectVitaPanel : public Panel
    {
    public:
        PackageProjectVitaPanel()
            : Panel("Package Project (Vita)")
        {}

        void OnImGuiRender(bool* opened, float deltaTime) override;

    private:
        void Package();
        bool SetupTempFolder();
        bool Compile();
        bool CopyBinaries();

    private:
        std::atomic<uint8_t> m_CaughtError = 0;
        std::filesystem::path m_TargetBuildPath, m_BinFilePath;
        std::future<void> m_CompileFuture;
    };
}