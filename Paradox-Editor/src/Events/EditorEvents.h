#pragma once

#include <Paradox/Events/Event.h>
#include <sstream>

namespace Paradox
{
	class ProjectChangedEvent : public Event
	{
	public:
		EVENT_CLASS_TYPE(ProjectChanged)
		EVENT_CLASS_CATEGORY(EventCategoryEditor)
	};
}