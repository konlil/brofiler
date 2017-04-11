#include "ProfilerServer.h"
#include "Counter.h"

namespace Brofiler
{
	CounterMgr::CounterMgr() : new_counter_idx_since_last_dump(-1)
	{

	}

	CounterMgr::~CounterMgr()
	{

	}

	uint8_t CounterMgr::GetCounterIndex(std::string name)
	{
		Platform::ScopedGuard guard(lock);
		if (counter_ids.find(name) == counter_ids.end())
		{
			Counter* c = new Counter(name, 0);
			counter_ids[name] = counters.size();
			if(new_counter_idx_since_last_dump < 0)
				new_counter_idx_since_last_dump = counters.size();

			counters.push_back(c);
		}
		return counter_ids[name];
	}

	void CounterMgr::Set(uint8_t idx, double v)
	{
		Platform::ScopedGuard guard(lock);
		counters[idx]->value = v;
	}

	void CounterMgr::IncAt(uint8_t idx, double v)
	{
		Platform::ScopedGuard guard(lock);
		counters[idx]->value += v;
	}

	void CounterMgr::Reset(uint8_t idx)
	{
		Platform::ScopedGuard guard(lock);
		counters[idx]->value = 0;
	}

	void CounterMgr::ResetNewCounterIdx()
	{
		new_counter_idx_since_last_dump = 0;
	}

	void CounterMgr::Dump(uint64_t frame_id)
	{
		Platform::ScopedGuard guard(lock);

		OutputDataStream stream;

		stream << frame_id;

		unsigned char has_new_counter = (new_counter_idx_since_last_dump >= 0) ? 1 : 0;
		stream << has_new_counter;
		if (has_new_counter > 0)
		{
			stream << (counters.size() - new_counter_idx_since_last_dump);
			for (size_t i = new_counter_idx_since_last_dump; i < counters.size(); i++)
			{
				const Counter* c = counters[i];
				stream << c->name;
				stream << i;
			}

			new_counter_idx_since_last_dump = -1;
		}

		stream << counters.size();
		for (int i = 0; i < counters.size(); i++)
		{
			const Counter* c = counters[i];
			stream << c->value;
		}

		Server::Get().Send(DataResponse::ReportCounters, stream);
	}

///////////////////////////////////////////////////////////////////////////////////////////
	uint8_t CounterAPI::GetCounterIndex(const char* name)
	{
		return Brofiler::CounterMgr::Get().GetCounterIndex(name);
	}

	void CounterAPI::IncCounter(uint8_t idx, double v)
	{
		Brofiler::CounterMgr::Get().IncAt(idx, v);
	}

	void CounterAPI::SetCounter(uint8_t idx, double v)
	{
		Brofiler::CounterMgr::Get().Set(idx, v);
	}
///////////////////////////////////////////////////////////////////////////////////////////
}