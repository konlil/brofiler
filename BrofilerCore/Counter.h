#pragma once
#include <map>
#include <vector>
#include "Brofiler.h"
#include "Singleton.h"

#include "Config.h"
#include "Platform/Platform.h"

#define MAX_COUNTERS 255

namespace Brofiler
{
	struct Counter
	{
		Counter(std::string& n, double v) : name(n), value(v) {}
		std::string name;
		double value;
	};

	class CounterMgr
	{
	private:
		Platform::Mutex lock;

		std::map<std::string, uint8_t> counter_ids;
		std::vector<Counter*> counters;
		int new_counter_idx_since_last_dump;

		CounterMgr();
		~CounterMgr();
		friend struct Singleton<CounterMgr>;

	public:
		static BRO_INLINE CounterMgr& Get() { return Singleton<CounterMgr>::instance(); }

		uint8_t GetCounterIndex(std::string name);
		void Set(uint8_t idx, double v);
		void IncAt(uint8_t idx, double v);
		void Reset(uint8_t idx);
		void Dump(uint64_t frame_id);

		void ResetNewCounterIdx();
	};

}