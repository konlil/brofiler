#include <cstring>
#include "Event.h"
#include "Core.h"
#include "Platform/Platform.h"
#include "EventDescriptionBoard.h"

namespace Brofiler
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static Platform::Mutex g_lock;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//EventDescription* EventDescription::Create(uint32_t mask, const char* eventName, const char* fileName, const unsigned long fileLine, const unsigned long eventColor /*= Color::Null*/)
EventDescription* EventDescription::Create(const char* eventName, const char* fileName, const unsigned long fileLine, const unsigned long eventColor /*= Color::Null*/)
{
	Platform::ScopedGuard guard(g_lock);

	EventDescription* result = EventDescriptionBoard::Get().CreateDescription();
	result->name = eventName;
	result->file = fileName;
	result->line = fileLine;
	result->color = eventColor;
	//result->c_mask = mask;
	return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventDescription::EventDescription() : isSampling(false), name(""), file(""), line(0), color(0) //, c_mask(0)
{
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventDescription& EventDescription::operator=(const EventDescription&)
{
	BRO_FAILED("It is pointless to copy EventDescription. Please, check you logic!"); return *this; 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventData* Event::Start(const EventDescription& description)
{
	if (!Core::Get().IsCurrentThreadRegistered())
	{
		Platform::Log("[Brofiler][Warning] Event (%s @%s,line %d) start from unregistered thread. \n", description.name, description.file, description.line);
		return nullptr;
	}

	/*if (!Core::Get().IsValidMask(description.c_mask))
	{
		return nullptr;
	}*/

	EventData* result = nullptr;

	EVENT_STORAGE_PTR_SLOT slot = Core::storage.get();
	if (slot)
	{
		EVENT_STORAGE_PTR storage = *slot;
		if (storage)
		{
			result = &storage->NextEvent();
			result->description = &description;
			result->Start();
		}
	}
	return result;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Event::Stop(EventData& data)
{
	data.Stop();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream & operator<<(OutputDataStream &stream, const EventDescription &ob)
{
	unsigned char flags = (ob.isSampling ? 0x1 : 0);
	return stream << ob.name << ob.file << ob.line << ob.color << flags;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator<<(OutputDataStream& stream, const EventTime& ob)
{
	return stream << ob.start << ob.finish;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator<<(OutputDataStream& stream, const EventData& ob)
{
	return stream << (EventTime)(ob) << ob.description->index;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator<<(OutputDataStream& stream, const SyncData& ob)
{
	return stream << (EventTime)(ob) << ob.core << ob.reason << ob.newThreadId;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Category::Category( const EventDescription& description) : Event(description)
{
	if (data)
	{
		EVENT_STORAGE_PTR_SLOT slot = Core::storage.get();
		if (slot)
		{
			EVENT_STORAGE_PTR storage = *slot;
			if (storage)
			{
				storage->RegisterCategory(*data);
			}
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
