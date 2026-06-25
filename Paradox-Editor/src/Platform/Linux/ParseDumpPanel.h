#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
    class ParseDumpPanel : public Panel
    {
    public:
        ParseDumpPanel()
            : Panel("Parse Dump") {}

        void OnImGuiRender() override;

    private:
        void ParseDump();
        std::string ParseAddress(const std::string& address);

    private:
        std::filesystem::path m_ExecutablePath;
        std::string m_RawDump, m_ParsedDump;
    };
}