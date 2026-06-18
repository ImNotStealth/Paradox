#pragma once

#include <Paradox/Core/Base.h>
#include <Paradox/Events/Event.h>

#include <imgui.h>

namespace Paradox
{
	class Panel
	{
	public:
		Panel(const std::string& name) 
			: m_Name(name) {}

		virtual void OnImGuiRender() = 0;
		virtual void OnEvent(Event& event) {}

		inline const std::string& GetName() const { return m_Name; }

	protected:
		std::string m_Name;
	};
}