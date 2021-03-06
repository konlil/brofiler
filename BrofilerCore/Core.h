#pragma once
#include "Brofiler.h"
#include "Singleton.h"
#include "Event.h"
#include "MemoryPool.h"
#include "Serialization.h"
//#include "CallstackCollector.h"
//#include "SysCallCollector.h"

#include <map>

namespace Brofiler
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct SymbolEngine;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ScopeHeader
{
	EventTime event;
	uint32_t boardNumber;
	int32_t threadNumber;

	ScopeHeader();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator << ( OutputDataStream& stream, const ScopeHeader& ob);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ScopeData
{
	ScopeHeader header;
	std::vector<EventData> categories;
	std::vector<EventData> events;

	void AddEvent(const EventData& data)
	{
		events.push_back(data);
		if (data.description->color != Color::Null)
		{
			categories.push_back(data);
		}
	}

	void InitRootEvent(const EventData& data)
	{
		header.event = data;
		AddEvent(data);
	}

	void Send();
	void Clear();
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator << ( OutputDataStream& stream, const ScopeData& ob);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef MemoryPool<EventData, 1024> EventBuffer;
typedef MemoryPool<const EventData*, 32> CategoryBuffer;
//typedef MemoryPool<SyncData, 1024> SynchronizationBuffer;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EventStorage
{
	EventBuffer eventBuffer;
	CategoryBuffer categoryBuffer; 
	//SynchronizationBuffer synchronizationBuffer;

	//TODO: sampling is deprecated..
	Platform::Atomic<uint32_t> isSampling;

	EventStorage();

	BRO_INLINE EventData& NextEvent() 
	{
		return eventBuffer.Add(); 
	}

	BRO_INLINE void RegisterCategory(const EventData& eventData) 
	{ 
		categoryBuffer.Add() = &eventData;
	}

	// Free all temporary memory
	void Clear(bool preserveContent)
	{
		eventBuffer.Clear(preserveContent);
		categoryBuffer.Clear(preserveContent);
		//synchronizationBuffer.Clear(preserveContent);
	}

	void Reset()
	{
		Clear(true);
	}
};
#define EVENT_STORAGE_PTR EventStorage*
#define EVENT_STORAGE_PTR_SLOT EventStorage**
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ThreadDescription
{
	const char* name;
	THREADID threadID;
	bool fromOtherProcess;

	ThreadDescription(const char* threadName, const THREADID& id, bool _fromOtherProcess)
		: name(threadName)
		, threadID(id)
		, fromOtherProcess(_fromOtherProcess)
	{}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ThreadEntry
{
	ThreadDescription description;
	EventStorage storage;
	EVENT_STORAGE_PTR_SLOT threadTLS;

	bool isAlive;

	ThreadEntry(const ThreadDescription& desc, EVENT_STORAGE_PTR_SLOT tls) : description(desc), threadTLS(tls), isAlive(true) {}
	void Activate(bool isActive);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef std::vector<ThreadEntry*> ThreadList;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct CaptureStatus
{
	enum Type
	{
		OK = 0,
		ERR_TRACER_ALREADY_EXISTS = 1,
		ERR_TRACER_ACCESS_DENIED = 2,
		FAILED = 3,
	};
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Core
{
	Platform::Mutex lock;

	THREADID mainThreadID;

	ThreadList threads;

	int64_t progressReportedLastTimestampMS;

	std::vector<EventTime> frames;

	uint64_t frame_id_started;
	uint64_t frame_id;

	EventData* frame_event_data;
	EventDescription* frame_event_desc;

	int filter_threshold;	// filter threshold (ms)

	void UpdateEvents();
	void Update();

	Core();
	~Core();

	friend struct Singleton<Core>;

	void DumpCounters();
	void DumpCapturingProgress();
	void SendHandshakeResponse(CaptureStatus::Type status);

	//������д����
	void DumpEvents(const EventStorage& entry, const EventTime& timeSlice, ScopeData& scope);
	void DumpThread(const ThreadEntry& entry, const EventTime& timeSlice, ScopeData& scope);

	void CleanupThreads();
public:
	void Init(const char* main_thread_name);

	void Activate(bool active);
	bool isActive;

	void ActivateCounters(bool active);
	bool isCounterActive;

	const THREADID GetMainThreadID() { return mainThreadID; }

	// Active Frame (is used as buffer)
	//static BF_THREAD_LOCAL EventStorage* storage;
    static Platform::TLSStorage<EVENT_STORAGE_PTR> storage;

	// Resolves symbols
	SymbolEngine* symbolEngine;

	// Controls GPU activity
	// Graphics graphics;

	// Returns thread collection
	const std::vector<ThreadEntry*>& GetThreads() const;

	// Serialize and send current profiling progress
	void DumpProgress(const char* message = "");

	// Too much time from last report
	bool IsTimeToReportProgress() const;

	// Serialize and send frames
	void DumpFrames();

	// Registers thread and create EventStorage
	//bool RegisterThread(const ThreadDescription& description, EventStorage** slot);
    bool RegisterThread(const ThreadDescription& description);

	// UnRegisters thread
	bool UnRegisterThread(THREADID threadId);

	// Check is registered thread
	bool IsCurrentThreadRegistered();

	// NOT Thread Safe singleton (performance)
	static BRO_INLINE Core& Get() { return Singleton<Core>::instance(); }

	// Main Update Function
	void BeginFrame();
	void EndFrame();

	// Set Filter Threshold
	void BRO_INLINE SetFilterThreshold(int v) { filter_threshold = v; }

	// Get Active ThreadID
	//static BRO_INLINE uint32 GetThreadID() { return Get().mainThreadID; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
