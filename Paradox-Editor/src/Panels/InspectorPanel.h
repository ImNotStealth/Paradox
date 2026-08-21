#pragma once

#include "Panels/Panel.h"

#include <Paradox.h>
#include <typeindex>

namespace Paradox
{
	class InspectorPanel : public Panel
	{
	public:
		InspectorPanel();

		void OnImGuiRender(bool* opened) override;

	private:
		template<typename T>
		void RegisterInspector(const std::string& name, std::function<void(T&)> func);

		template<typename T, typename Func>
		void DrawContainer(const std::string& name, Entity entity, Func func);

		void OnTransformComponent(Entity& entity);
		void OnSpriteComponent(SpriteComponent& comp);

	private:
		class EditorApp& m_AppRef;
		class Scene* m_Scene = nullptr;

		struct InspectorEntry
		{
			std::function<void(Entity&)> popupFunc;
			std::function<void(Entity&)> func;
			std::string name;
		};
		std::unordered_map<std::type_index, InspectorEntry> m_Inspectors;
	};
}