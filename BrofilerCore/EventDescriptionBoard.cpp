#include "EventDescriptionBoard.h"
#include "Event.h"
#include "Singleton.h"

namespace Brofiler
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Platform::Mutex g_lock;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventDescriptionBoard& EventDescriptionBoard::Get()
{
	return Singleton<EventDescriptionBoard>::instance();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const std::vector<EventDescription*>& EventDescriptionBoard::GetEvents() const
{
	return board;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventDescriptionBoard::~EventDescriptionBoard()
{
	for(auto it = board.begin(); it != board.end(); ++it)
	{
		EventDescription* desc = *it;
		delete desc;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventDescription* EventDescriptionBoard::CreateDescription()
{
	Platform::ScopedGuard guard(g_lock);
	EventDescription* desc = new EventDescription();
	desc->index = (unsigned long)board.size();
	board.push_back(desc);
	return desc;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator << ( OutputDataStream& stream, const EventDescriptionBoard& ob)
{
	Platform::ScopedGuard guard(g_lock);
	const std::vector<EventDescription*>& events = ob.GetEvents();

	stream << (uint32_t)events.size();

	for(auto it = events.begin(); it != events.end(); ++it)
	{
		const EventDescription* desc = *it;
		stream << *desc;
	}

	return stream;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
