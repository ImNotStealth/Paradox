#include "pxpch.h"
#include "RuntimeConfig.h"

namespace Paradox
{
	void RuntimeConfig::Serialize(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
			std::filesystem::create_directories(filePath.parent_path());

		std::ofstream file(filePath.string(), std::ios::out | std::ios::binary);
		if (!file.is_open())
		{
			PX_CORE_ERROR("Failed to open RuntimeConfig file: {0}", filePath.string());
			return;
		}

		WriteString(file, m_Properties.windowProperties.title);
		file.write((char*)&m_Properties.windowProperties.width, sizeof(uint32_t));
		file.write((char*)&m_Properties.windowProperties.height, sizeof(uint32_t));
		WriteString(file, m_Properties.windowProperties.iconPath);
		file.write((char*)&m_Properties.windowProperties.fullscreen, sizeof(bool));
		file.write((char*)&m_Properties.windowProperties.maximized, sizeof(bool));
		file.write((char*)&m_Properties.windowProperties.vsync, sizeof(bool));
		WriteString(file, GraphicsContext::GraphicsAPIToString(m_Properties.windowProperties.graphicsAPI));
		WriteString(file, m_Properties.startScene.ToString());
	}

	bool RuntimeConfig::Deserialize(const std::filesystem::path& filePath)
	{
		if (!std::filesystem::exists(filePath))
		{
			PX_ERROR("RuntimeConfig file does not exist: {0}", filePath.string());
			return false;
		}

		std::ifstream file(filePath.string(), std::ios::in | std::ios::binary);
		if (!file.is_open())
		{
			PX_ERROR("Failed to open RuntimeConfig file: {0}", filePath.string());
			return false;
		}

		m_Properties.windowProperties.title = ReadString(file);
		file.read((char*)&m_Properties.windowProperties.width, sizeof(uint32_t));
		file.read((char*)&m_Properties.windowProperties.height, sizeof(uint32_t));
		m_Properties.windowProperties.iconPath = ReadString(file);
		file.read((char*)&m_Properties.windowProperties.fullscreen, sizeof(bool));
		file.read((char*)&m_Properties.windowProperties.maximized, sizeof(bool));
		file.read((char*)&m_Properties.windowProperties.vsync, sizeof(bool));
		m_Properties.windowProperties.graphicsAPI = GraphicsContext::StringToGraphicsAPI(ReadString(file));
		m_Properties.startScene = UUID(ReadString(file));
		return false;
	}

	void RuntimeConfig::WriteString(std::ostream& out, const std::string& str)
	{
		uint32_t size = (uint32_t)str.size();
		out.write((char*)&size, sizeof(uint32_t));
		out.write(str.c_str(), size);
	}

	std::string RuntimeConfig::ReadString(std::istream& in)
	{
		uint32_t size;
		in.read((char*)&size, sizeof(uint32_t));

		std::string str = std::string(size, '\0');
		in.read(str.data(), size);
		return str;
	}
}