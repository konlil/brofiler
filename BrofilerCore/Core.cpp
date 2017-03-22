#include "Core.h"
#include "Common.h"
#include "Event.h"
#include "ProfilerServer.h"
#include "EventDescriptionBoard.h"
#include "Thread.h"

#include "Platform/Platform.h"
//#include "Platform/SchedulerTrace.h"
//#include "Platform/SamplingProfiler.h"
//#include "Platform/SymbolEngine.h"


extern "C" Brofiler::EventData* NextEvent()
{
    Brofiler::EventStorage* storage = Brofiler::Core::storage.get();
	if (storage)
	{
		return &storage->NextEvent();
	}

	return nullptr;
}


namespace Brofiler
{

Platform::TLSStorage<EventStorage> Core::storage;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
int64_t GetHighPrecisionTime()
{
	return Platform::TimeMicroSeconds();
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpProgress(const char* message)
{
	progressReportedLastTimestampMS = Platform::TimeMilliSeconds();

	OutputDataStream stream;
	stream << message;

	Server::Get().Send(DataResponse::ReportProgress, stream);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpEvents(const EventStorage& entry, const EventTime& timeSlice, ScopeData& scope)
{
	if (!entry.eventBuffer.IsEmpty())
	{
		const EventData* rootEvent = nullptr;

		entry.eventBuffer.ForEach([&](const EventData& data)
		{
			if (data.finish >= data.start && data.start >= timeSlice.start && timeSlice.finish >= data.finish)
			{
				if (!rootEvent)
				{
					rootEvent = &data;
					scope.InitRootEvent(*rootEvent);
				} 
				else if (rootEvent->finish < data.finish)
				{
					scope.Send();

					rootEvent = &data;
					scope.InitRootEvent(*rootEvent);
				}
				else
				{
					scope.AddEvent(data);
				}
			}
		});

		scope.Send();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpThread(const ThreadEntry& entry, const EventTime& timeSlice, ScopeData& scope)
{
	// Events
	DumpEvents(entry.storage, timeSlice, scope);

	if (!entry.storage.synchronizationBuffer.IsEmpty())
	{
		OutputDataStream synchronizationStream;
		synchronizationStream << scope.header.boardNumber;
		synchronizationStream << scope.header.threadNumber;
		synchronizationStream << entry.storage.synchronizationBuffer;
		Server::Get().Send(DataResponse::Synchronization, synchronizationStream);
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpFrames()
{
	if (frames.empty() || threads.empty())
		return;

	DumpProgress("Collecting Frame Events...");

	//Graphics::Image image;
	//graphics.GetScreenshot(image);

	uint32_t mainThreadIndex = 0;

	for (size_t i = 0; i < threads.size(); ++i)
	{
		if (threads[i]->description.threadID == mainThreadID)
		{
			mainThreadIndex = (uint32_t)i;
		}
	}

	EventTime timeSlice;
	timeSlice.start = frames.front().start;
	timeSlice.finish = frames.back().finish;

	OutputDataStream boardStream;

	static uint32_t boardNumber = 0;
	boardStream << ++boardNumber;
	boardStream << Platform::GetFrequency();
	boardStream << timeSlice;
	boardStream << threads;
	boardStream << mainThreadIndex;
	boardStream << EventDescriptionBoard::Get();
	Server::Get().Send(DataResponse::FrameDescriptionBoard, boardStream);

	ScopeData threadScope;
	threadScope.header.boardNumber = (uint32_t)boardNumber;

	for (size_t i = 0; i < threads.size(); ++i)
	{
		threadScope.header.threadNumber = (uint32_t)i;
		DumpThread(*threads[i], timeSlice, threadScope);
	}

	frames.clear();
	CleanupThreads();

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::CleanupThreads()
{
	Platform::ScopedGuard guard(lock);

	for (ThreadList::iterator it = threads.begin(); it != threads.end();)
	{
		if (!(*it)->isAlive)
		{
			(*it)->~ThreadEntry();
			Platform::Memory::Free(*it);
			it = threads.erase(it);
		}
		else
		{
			++it;
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Core::Core() : progressReportedLastTimestampMS(0), isActive(false)
{
	mainThreadID = Platform::Thread::CurrentThreadID();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::Update()
{
	Platform::ScopedGuard guard(lock);
	
	if (isActive)
	{
		if (!frames.empty())
			frames.back().Stop();

		if (IsTimeToReportProgress())
			DumpCapturingProgress();		
	}

	UpdateEvents();

	if (isActive)
	{
		frames.push_back(EventTime());
		frames.back().Start();
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::UpdateEvents()
{
	Server::Get().Update();
}

#define THREAD_DISABLED_BIT (1)
#define THREAD_ENABLED_BIT (2)

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::Activate( bool active )
{
	if (isActive != active)
	{
		isActive = active;

		for(auto it = threads.begin(); it != threads.end(); ++it)
		{
			ThreadEntry* entry = *it;
			entry->Activate(active);
		}
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpCapturingProgress()
{
	std::stringstream stream;

	if (isActive)
		stream << "Capturing Frame " << (uint32_t)frames.size() << std::endl;

	DumpProgress(stream.str().c_str());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Core::IsTimeToReportProgress() const
{
	return Platform::TimeMilliSeconds() > progressReportedLastTimestampMS + 200;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::SendHandshakeResponse(CaptureStatus::Type status)
{
	OutputDataStream stream;
	stream << (uint32_t)status;
	Server::Get().Send(DataResponse::Handshake, stream);
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Core::IsRegistredThread(THREADID id)
{
	Platform::ScopedGuard guard(lock);
	for (ThreadList::iterator it = threads.begin(); it != threads.end(); ++it)
	{
		ThreadEntry* entry = *it;
		if (entry->description.threadID == id)
		{
			return true;
		}
	}
	return false;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//bool Core::RegisterThread(const ThreadDescription& description, EventStorage** slot)
bool Core::RegisterThread(const ThreadDescription& description)
{
	Platform::ScopedGuard guard(lock);
    
    EventStorage* storage = Core::storage.get();
    if ( !storage ){
        storage = Core::storage.alloc(1);
    }
    EventStorage** slot = &storage;
    
	ThreadEntry* entry = new (Platform::Memory::Alloc(sizeof(ThreadEntry), BRO_CACHE_LINE_SIZE)) ThreadEntry(description, slot);
	threads.push_back(entry);

	if (isActive && slot != nullptr && *slot != nullptr)
		*slot = &entry->storage;

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool Core::UnRegisterThread(THREADID threadID)
{
	Platform::ScopedGuard guard(lock);
	for (ThreadList::iterator it = threads.begin(); it != threads.end(); ++it)
	{
		ThreadEntry* entry = *it;
		if (entry->description.threadID == threadID && entry->isAlive)
		{
			if (!isActive)
			{
				entry->~ThreadEntry();
				Platform::Memory::Free(entry);
				threads.erase(it);
				return true;
			}
			else
			{
				entry->isAlive = false;
				return true;
			}
		}
	}

	return false;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Core::~Core()
{
	Platform::ScopedGuard guard(lock);

	for (ThreadList::iterator it = threads.begin(); it != threads.end(); ++it)
	{
		(*it)->~ThreadEntry();
		Platform::Memory::Free((*it));
	}
	threads.clear();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
const std::vector<ThreadEntry*>& Core::GetThreads() const
{
	return threads;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// static TLS for each Thread
//BF_THREAD_LOCAL EventStorage* Core::storage = nullptr;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
Core Core::notThreadSafeInstance;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
ScopeHeader::ScopeHeader() : boardNumber(0), threadNumber(0)
{

}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator<<(OutputDataStream& stream, const ScopeHeader& header)
{
	return stream << header.boardNumber << header.threadNumber << header.event;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator<<(OutputDataStream& stream, const ScopeData& ob)
{
	return stream << ob.header << ob.categories << ob.events;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator<<(OutputDataStream& stream, const ThreadDescription& description)
{
	return stream << description.threadID << description.name;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator<<(OutputDataStream& stream, const ThreadEntry* entry)
{
	return stream << entry->description;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BROFILER_API void NextFrame()
{
	return Core::NextFrame();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BROFILER_API bool IsActive()
{
	return Core::Get().isActive;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//BROFILER_API EventStorage** GetEventStorageSlotForCurrentThread()
//{
//	return &Core::Get().storage.get();
//}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BROFILER_API bool RegisterThread(const char* name)
{
	return Core::Get().RegisterThread(ThreadDescription(name, Platform::Thread::CurrentThreadID(), false));
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BROFILER_API bool UnRegisterThread()
{
	return Core::Get().UnRegisterThread(Platform::Thread::CurrentThreadID());
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EventStorage::EventStorage(): isSampling(0)
{
	 
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ThreadEntry::Activate(bool isActive)
{
	if (!isAlive)
		return;

	if (isActive)
		storage.Clear(true);

	if (threadTLS != nullptr)
	{
		*threadTLS = isActive ? &storage : nullptr;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool IsSleepOnlyScope(const ScopeData& scope)
{
	if (!scope.categories.empty())
		return false;

	const std::vector<EventData>& events = scope.events;
	for(auto it = events.begin(); it != events.end(); ++it)
	{
		const EventData& data = *it;

		if (data.description->color != Color::White)
		{
			return false;
		}
	}

	return true;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScopeData::Send()
{
	if (!events.empty() || !categories.empty())
	{
		if (!IsSleepOnlyScope(*this))
		{
			OutputDataStream frameStream;
			frameStream << *this;
			Server::Get().Send(DataResponse::EventFrame, frameStream);
		}
	}

	Clear();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void ScopeData::Clear()
{
	events.clear();
	categories.clear();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
