#pragma once
#include <map>
#include <vector>
#include <float.h>
#include "Brofiler.h"
#include "Singleton.h"

#include "Config.h"
#include "Platform/Platform.h"

#define MAX_COUNTERS 255

namespace Brofiler
{
	struct Counter
	{
		Counter(const std::string& n) :
			name(n), min_value(-DBL_MAX), max_value(DBL_MAX) {}
		Counter(const std::string& n, double vmin, double vmax) : 
			name(n), min_value(vmin), max_value(vmax) {}
		std::string name;
		double min_value;
		double max_value;
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

		uint8_t DeclCounter(const std::string& name, double vmin, double vmax);
		uint8_t GetCounterIndex(const std::string& name);
		void Set(uint8_t idx, double v);
		void IncAt(uint8_t idx, double v);
		void Reset(uint8_t idx);
		void Dump(uint64_t frame_id);

		void ResetNewCounterIdx();
	};

}
