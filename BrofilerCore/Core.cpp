#include "Core.h"
#include "Common.h"
#include "Event.h"
#include "Counter.h"
#include "ProfilerServer.h"
#include "EventDescriptionBoard.h"
#include "Thread.h"

#include "Platform/Platform.h"
//#include "Platform/SchedulerTrace.h"
//#include "Platform/SamplingProfiler.h"
//#include "Platform/SymbolEngine.h"


extern "C" Brofiler::EventData* NextEvent()
{
    Brofiler::EventStorage** slot = Brofiler::Core::storage.get();
	if (slot)
	{
		return &(*slot)->NextEvent();
	}

	return nullptr;
}


namespace Brofiler
{

Platform::TLSStorage<EVENT_STORAGE_PTR> Core::storage;

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

		//Platform::Log("[Brofiler]DumpEvents ------------ %d \n", entry.eventBuffer.Size());
		entry.eventBuffer.ForEach([&](const EventData& data)
		{
			//Platform::Log("[Brofiler]DumpEvents, duration: %f ms, %lld, %lld, %s \n", data.Duration(), data.start, data.finish, data.description->name);
			if (data.finish >= data.start && data.start >= timeSlice.start && timeSlice.finish >= data.finish)
			{
				if (!rootEvent)
				{
					rootEvent = &data;
					scope.InitRootEvent(*rootEvent);
				} 
				else if (rootEvent->finish < data.finish)
				{
					//Platform::Log("[Brofiler]DumpEvents, --- change root: %f ms, %lld, %lld, %s \n", data.Duration(), data.start, data.finish, data.description->name);
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

		//Platform::Log("[Brofiler]DumpEvents, header duration: %f ms\n", rootEvent->Duration());
		scope.Send();
	}
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpThread(const ThreadEntry& entry, const EventTime& timeSlice, ScopeData& scope)
{
	// Events
	DumpEvents(entry.storage, timeSlice, scope);

	/*if (!entry.storage.synchronizationBuffer.IsEmpty())
	{
		OutputDataStream synchronizationStream;
		synchronizationStream << scope.header.boardNumber;
		synchronizationStream << scope.header.threadNumber;
		synchronizationStream << entry.storage.synchronizationBuffer;
		Server::Get().Send(DataResponse::Synchronization, synchronizationStream);
	}*/
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
Core::Core() : progressReportedLastTimestampMS(0), 
	isActive(false), isCounterActive(false), frame_id(0),
	filter_threshold(0)
{
}

void Core::Init()
{
	mainThreadID = Platform::Thread::CurrentThreadID();
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::Update()
{
	Platform::ScopedGuard guard(lock);

	if (isCounterActive)
	{
		DumpCounters();
	}
	
	//bool filtered = false;
	if (isActive)
	{
		if (!frames.empty())
		{
			frames.back().Stop();
			//EventTime& time = frames.back();
			//Platform::Log("[Brofiler]Frame time: %f ms\n", (time.finish - time.start) / 1000.0);
		}
	}
	
	if (IsTimeToReportProgress())
		DumpCapturingProgress();		

	UpdateEvents();

	if (isActive)
	{
		frames.push_back(EventTime());
		frames.back().Start();
	}

	frame_id++;
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

void Core::ActivateCounters(bool active)
{
	if (isCounterActive != active)
	{
		isCounterActive = active;
		Brofiler::CounterMgr::Get().ResetNewCounterIdx();
	}

	if (active)
		frame_id_started = frame_id;
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpCounters()
{
	static int64_t counterReportedLastTimestampMS = 0;
	int64_t now = Platform::TimeMilliSeconds();
	if ( now > counterReportedLastTimestampMS + 500)
	{
		Brofiler::CounterMgr::Get().Dump(frame_id);
		counterReportedLastTimestampMS = now;
	}
}
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Core::DumpCapturingProgress()
{
	std::stringstream stream;

	if (isActive)
	{
		stream << "Capturing Frame " << (uint32_t)frames.size() << std::endl;
	}
	else if (isCounterActive)
	{
		stream << "Capturing Frame " << (uint32_t)(frame_id - frame_id_started) << std::endl;
	}

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
bool Core::IsCurrentThreadRegistered()
{
	Platform::ScopedGuard guard(lock);
	THREADID cur_id = Platform::Thread::CurrentThreadID();
	for (ThreadList::iterator it = threads.begin(); it != threads.end(); ++it)
	{
		ThreadEntry* entry = *it;
		if (entry->description.threadID == cur_id)
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
    
	EVENT_STORAGE_PTR_SLOT slot = Core::storage.get();
    if ( slot == NULL){
        slot = Core::storage.alloc(1);
		*slot = 0;
    }
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
//Core Core::notThreadSafeInstance;
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
BROFILER_API void Init()
{
	return Core::Get().Init();
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
BROFILER_API bool IsCounterActive()
{
	return Core::Get().isCounterActive;
}
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
		//TODO: cannot understand this line..
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
		//TODO
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
