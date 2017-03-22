#pragma once

#define NOCOPYABLE(TYPE) \
	private: \
		TYPE (const TYPE &); \
		void operator= (const TYPE &); \
	public: \

//#define BF_THREAD_LOCAL thread_local

struct LocalTime
{
	int year, month, day;
	int hour, minute, second, millisecond;
};

