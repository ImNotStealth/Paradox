#pragma once

#include "Panels/Panel.h"

namespace Paradox
{
	class YesNoPanel : public Panel
	{
	public:
		using Callback = std::function<void(bool value)>;

		YesNoPanel(const std::string& title, const std::string& message, Callback callback, const std::string& confirm = "Confirm", const std::string& cancel = "Cancel")
			: Panel(title + "##YesNo"), m_Message(message), m_Callback(std::move(callback)), m_Confirm(confirm), m_Cancel(cancel) { }

		void OnImGuiRender(bool* opened, float deltaTime) override;

	private:
		std::string m_Message, m_Confirm, m_Cancel;
		Callback m_Callback;
	};
}