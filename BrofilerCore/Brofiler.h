#pragma once

#include <stdint.h>

#if defined(__clang__) || defined(__GNUC__)
#define BRO_GCC (1)
#elif defined(_MSC_VER)
#define BRO_MSVC (1)
#else
#error Compiler not supported
#endif


#if BRO_GCC
#define __BRO_DECORATED_FUNCTION__ __PRETTY_FUNCTION__
#elif BRO_MSVC
#define __BRO_DECORATED_FUNCTION__ __FUNCSIG__
#else
#error Compiler not supported
#endif

#if !defined(USE_BROFILER) && (!defined(_FINALRELEASE))
	#define USE_BROFILER 1
#endif




#if USE_BROFILER
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// EXPORTS 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef PROFILER_EXPORTS
#define BROFILER_API __declspec(dllexport)
#else
#define BROFILER_API //__declspec(dllimport)
#endif
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BRO_CONCAT_IMPL(x, y) x##y
#define BRO_CONCAT(x, y) BRO_CONCAT_IMPL(x, y)

#if BRO_MSVC
#define BRO_INLINE __forceinline
#elif BRO_GCC
#define BRO_INLINE __attribute__((always_inline)) inline
#else
#error Compiler is not supported
#endif


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

namespace Brofiler
{
	// Source: http://msdn.microsoft.com/en-us/library/system.windows.media.colors(v=vs.110).aspx
	// Image:  http://i.msdn.microsoft.com/dynimg/IC24340.png
	struct Color
	{
		enum
		{
			Null = 0x00000000,
			AliceBlue = 0xFFF0F8FF,
			AntiqueWhite = 0xFFFAEBD7,
			Aqua = 0xFF00FFFF,
			Aquamarine = 0xFF7FFFD4,
			Azure = 0xFFF0FFFF,
			Beige = 0xFFF5F5DC,
			Bisque = 0xFFFFE4C4,
			Black = 0xFF000000,
			BlanchedAlmond = 0xFFFFEBCD,
			Blue = 0xFF0000FF,
			BlueViolet = 0xFF8A2BE2,
			Brown = 0xFFA52A2A,
			BurlyWood = 0xFFDEB887,
			CadetBlue = 0xFF5F9EA0,
			Chartreuse = 0xFF7FFF00,
			Chocolate = 0xFFD2691E,
			Coral = 0xFFFF7F50,
			CornflowerBlue = 0xFF6495ED,
			Cornsilk = 0xFFFFF8DC,
			Crimson = 0xFFDC143C,
			Cyan = 0xFF00FFFF,
			DarkBlue = 0xFF00008B,
			DarkCyan = 0xFF008B8B,
			DarkGoldenRod = 0xFFB8860B,
			DarkGray = 0xFFA9A9A9,
			DarkGreen = 0xFF006400,
			DarkKhaki = 0xFFBDB76B,
			DarkMagenta = 0xFF8B008B,
			DarkOliveGreen = 0xFF556B2F,
			DarkOrange = 0xFFFF8C00,
			DarkOrchid = 0xFF9932CC,
			DarkRed = 0xFF8B0000,
			DarkSalmon = 0xFFE9967A,
			DarkSeaGreen = 0xFF8FBC8F,
			DarkSlateBlue = 0xFF483D8B,
			DarkSlateGray = 0xFF2F4F4F,
			DarkTurquoise = 0xFF00CED1,
			DarkViolet = 0xFF9400D3,
			DeepPink = 0xFFFF1493,
			DeepSkyBlue = 0xFF00BFFF,
			DimGray = 0xFF696969,
			DodgerBlue = 0xFF1E90FF,
			FireBrick = 0xFFB22222,
			FloralWhite = 0xFFFFFAF0,
			ForestGreen = 0xFF228B22,
			Fuchsia = 0xFFFF00FF,
			Gainsboro = 0xFFDCDCDC,
			GhostWhite = 0xFFF8F8FF,
			Gold = 0xFFFFD700,
			GoldenRod = 0xFFDAA520,
			Gray = 0xFF808080,
			Green = 0xFF008000,
			GreenYellow = 0xFFADFF2F,
			HoneyDew = 0xFFF0FFF0,
			HotPink = 0xFFFF69B4,
			IndianRed = 0xFFCD5C5C,
			Indigo = 0xFF4B0082,
			Ivory = 0xFFFFFFF0,
			Khaki = 0xFFF0E68C,
			Lavender = 0xFFE6E6FA,
			LavenderBlush = 0xFFFFF0F5,
			LawnGreen = 0xFF7CFC00,
			LemonChiffon = 0xFFFFFACD,
			LightBlue = 0xFFADD8E6,
			LightCoral = 0xFFF08080,
			LightCyan = 0xFFE0FFFF,
			LightGoldenRodYellow = 0xFFFAFAD2,
			LightGray = 0xFFD3D3D3,
			LightGreen = 0xFF90EE90,
			LightPink = 0xFFFFB6C1,
			LightSalmon = 0xFFFFA07A,
			LightSeaGreen = 0xFF20B2AA,
			LightSkyBlue = 0xFF87CEFA,
			LightSlateGray = 0xFF778899,
			LightSteelBlue = 0xFFB0C4DE,
			LightYellow = 0xFFFFFFE0,
			Lime = 0xFF00FF00,
			LimeGreen = 0xFF32CD32,
			Linen = 0xFFFAF0E6,
			Magenta = 0xFFFF00FF,
			Maroon = 0xFF800000,
			MediumAquaMarine = 0xFF66CDAA,
			MediumBlue = 0xFF0000CD,
			MediumOrchid = 0xFFBA55D3,
			MediumPurple = 0xFF9370DB,
			MediumSeaGreen = 0xFF3CB371,
			MediumSlateBlue = 0xFF7B68EE,
			MediumSpringGreen = 0xFF00FA9A,
			MediumTurquoise = 0xFF48D1CC,
			MediumVioletRed = 0xFFC71585,
			MidnightBlue = 0xFF191970,
			MintCream = 0xFFF5FFFA,
			MistyRose = 0xFFFFE4E1,
			Moccasin = 0xFFFFE4B5,
			NavajoWhite = 0xFFFFDEAD,
			Navy = 0xFF000080,
			OldLace = 0xFFFDF5E6,
			Olive = 0xFF808000,
			OliveDrab = 0xFF6B8E23,
			Orange = 0xFFFFA500,
			OrangeRed = 0xFFFF4500,
			Orchid = 0xFFDA70D6,
			PaleGoldenRod = 0xFFEEE8AA,
			PaleGreen = 0xFF98FB98,
			PaleTurquoise = 0xFFAFEEEE,
			PaleVioletRed = 0xFFDB7093,
			PapayaWhip = 0xFFFFEFD5,
			PeachPuff = 0xFFFFDAB9,
			Peru = 0xFFCD853F,
			Pink = 0xFFFFC0CB,
			Plum = 0xFFDDA0DD,
			PowderBlue = 0xFFB0E0E6,
			Purple = 0xFF800080,
			Red = 0xFFFF0000,
			RosyBrown = 0xFFBC8F8F,
			RoyalBlue = 0xFF4169E1,
			SaddleBrown = 0xFF8B4513,
			Salmon = 0xFFFA8072,
			SandyBrown = 0xFFF4A460,
			SeaGreen = 0xFF2E8B57,
			SeaShell = 0xFFFFF5EE,
			Sienna = 0xFFA0522D,
			Silver = 0xFFC0C0C0,
			SkyBlue = 0xFF87CEEB,
			SlateBlue = 0xFF6A5ACD,
			SlateGray = 0xFF708090,
			Snow = 0xFFFFFAFA,
			SpringGreen = 0xFF00FF7F,
			SteelBlue = 0xFF4682B4,
			Tan = 0xFFD2B48C,
			Teal = 0xFF008080,
			Thistle = 0xFFD8BFD8,
			Tomato = 0xFFFF6347,
			Turquoise = 0xFF40E0D0,
			Violet = 0xFFEE82EE,
			Wheat = 0xFFF5DEB3,
			White = 0xFFFFFFFF,
			WhiteSmoke = 0xFFF5F5F5,
			Yellow = 0xFFFFFF00,
			YellowGreen = 0xFF9ACD32,
		};
	};
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}


namespace Brofiler
{
BROFILER_API void Init(const char* name);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BROFILER_API int64_t GetHighPrecisionTime();
BROFILER_API void BeginFrame();
BROFILER_API void EndFrame();
BROFILER_API bool IsActive();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EventStorage;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
BROFILER_API bool RegisterThread(const char* name);
BROFILER_API bool UnRegisterThread();
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EventDescription;
struct Frame;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EventTime
{
	int64_t start;
	int64_t finish;

	BRO_INLINE void Start() { start  = Brofiler::GetHighPrecisionTime(); }
	BRO_INLINE void Stop() 	{ finish = Brofiler::GetHighPrecisionTime(); }
	BRO_INLINE float Duration() const { return (finish - start) / 1000.0f; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct EventData : public EventTime
{
	const EventDescription* description;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//struct BROFILER_API SyncData : public EventTime
//{
//	uint64_t core;
//	uint64_t newThreadId;
//	int8_t reason;
//};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct BROFILER_API EventDescription
{
	bool isSampling;		

	// HOT  \\
	// Have to place "hot" variables at the beginning of the class (here will be some padding)
	// COLD //

	const char* name;
	const char* file;
	uint32_t line;
	uint32_t index;
	uint32_t color;
	uint32_t c_mask;


	static uint32_t s_capture_mask;

	static bool IsValidMask(uint32_t mask);
	static void SetGlobalCaptureMask(uint32_t global_mask);
	static EventDescription* Create(uint32_t mask, const char* eventName, const char* fileName, const uint32_t fileLine, const uint32_t eventColor = Color::Null);
private:
	friend class EventDescriptionBoard;
	EventDescription();
	EventDescription& operator=(const EventDescription&);
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct BROFILER_API Event
{
	EventData* data;

	static EventData* Start(const EventDescription& description);
	static void Stop(EventData& data);

	Event(const EventDescription& description )
	{
		data = Start(description);
	}

	~Event()
	{
		if (data)
			Stop(*data);
	}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct BROFILER_API Category : public Event
{
	Category( const EventDescription& description );
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct ThreadScope
{
	ThreadScope(const char* name)
	{
		RegisterThread(name);
	}

	~ThreadScope()
	{
		UnRegisterThread();
	}
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct BROFILER_API CounterAPI
{
	static uint8_t DeclCounter(const char* name, double vmin, double vmax);
	static uint8_t GetCounterIndex(const char* name);
	static void SetCounter(uint8_t idx, double v);
	static void IncCounter(uint8_t idx, double v);
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}

#define BRO_UNUSED(x) (void)(x)


//Pre-defined Capture Mask
#define BROFILER_MASK_FRAME		(1)
#define BROFILER_MASK_UPDATE	(1 << 1)
#define BROFILER_MASK_RENDER	(1 << 2)
#define BROFILER_MASK_PHYSICS	(1 << 3)
#define BROFILER_MASK_NETWORK	(1 << 4)
#define BROFILER_MASK_AUDIO		(1 << 5)
#define BROFILER_MASK_RESERVED_COUNT 10

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define BROFILER_INIT(NAME) ::Brofiler::Init(NAME)

#define BROFILER_SET_CAPTURE_MASK(MASK) ::Brofiler::EventDescription::SetGlobalCaptureMask(MASK);

#define BROFILER_EVENT_WITH_MASK(MASK, NAME) static ::Brofiler::EventDescription* BRO_CONCAT(autogenerated_description_, __LINE__) = ::Brofiler::EventDescription::Create( MASK, NAME, __FILE__, __LINE__ ); \
							 ::Brofiler::Event BRO_CONCAT(autogenerated_event_, __LINE__)( *(BRO_CONCAT(autogenerated_description_, __LINE__)) ); \

#define BROFILER_EVENT(NAME) BROFILER_EVENT_WITH_MASK(0, NAME)

#define PROFILE BROFILER_EVENT( __BRO_DECORATED_FUNCTION__ )

#define BROFILER_INLINE_EVENT(NAME, CODE) { BROFILER_EVENT(NAME) CODE; }

#define BROFILER_CUSTOM_EVENT(DESCRIPTION) 	::Brofiler::Event BRO_CONCAT(autogenerated_event_, __LINE__)( *DESCRIPTION ); \

#define BROFILER_CATEGORY_WITH_MASK(MASK, NAME, COLOR) static ::Brofiler::EventDescription* BRO_CONCAT(autogenerated_description_, __LINE__) = ::Brofiler::EventDescription::Create( MASK, NAME, __FILE__, __LINE__, (unsigned long)COLOR ); \
										::Brofiler::Category BRO_CONCAT(autogenerated_event_, __LINE__)( *(BRO_CONCAT(autogenerated_description_, __LINE__)) ); \

#define BROFILER_CATEGORY(NAME, COLOR) BROFILER_CATEGORY_WITH_MASK(0, NAME, COLOR)

#define BROFILER_FRAME()	::Brofiler::EndFrame()
							 
#define BROFILER_THREAD(FRAME_NAME) ::Brofiler::ThreadScope brofilerThreadScope(FRAME_NAME); \
									BRO_UNUSED(brofilerThreadScope);						 \

#define BROFILER_START_THREAD(FRAME_NAME) ::Brofiler::RegisterThread(FRAME_NAME);

#define BROFILER_STOP_THREAD() ::Brofiler::UnRegisterThread();

// Compiled markup-level
#ifndef BROFILER_COMPILED_LEVEL
#define BROFILER_COMPILED_LEVEL 0
#endif
#define BROFILER_EVENT_WITH_MASK1(MASK, NAME) BROFILER_EVENT_WITH_MASK(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK2(MASK, NAME) BROFILER_EVENT_WITH_MASK(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK3(MASK, NAME) BROFILER_EVENT_WITH_MASK(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK4(MASK, NAME) BROFILER_EVENT_WITH_MASK(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK5(MASK, NAME) BROFILER_EVENT_WITH_MASK(MASK, NAME)
#define BROFILER_CATEGORY_WITH_MASK1(MASK, NAME, COLOR) BROFILER_CATEGORY_WITH_MASK(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK2(MASK, NAME, COLOR) BROFILER_CATEGORY_WITH_MASK(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK3(MASK, NAME, COLOR) BROFILER_CATEGORY_WITH_MASK(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK4(MASK, NAME, COLOR) BROFILER_CATEGORY_WITH_MASK(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK5(MASK, NAME, COLOR) BROFILER_CATEGORY_WITH_MASK(MASK, NAME, COLOR)
#define BROFILER_CATEGORY1(NAME, COLOR) BROFILER_CATEGORY(NAME, COLOR)
#define BROFILER_CATEGORY2(NAME, COLOR) BROFILER_CATEGORY(NAME, COLOR)
#define BROFILER_CATEGORY3(NAME, COLOR) BROFILER_CATEGORY(NAME, COLOR)
#define BROFILER_CATEGORY4(NAME, COLOR) BROFILER_CATEGORY(NAME, COLOR)
#define BROFILER_CATEGORY5(NAME, COLOR) BROFILER_CATEGORY(NAME, COLOR)
#if BROFILER_COMPILED_LEVEL == 0 || BROFILER_COMPILED_LEVEL > 4
//do nothing
#elif BROFILER_COMPILED_LEVEL < 0
#define BROFILER_EVENT_WITH_MASK1(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK2(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK3(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK4(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK5(MASK, NAME)
#define BROFILER_CATEGORY_WITH_MASK1(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK2(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK3(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK4(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK5(MASK, NAME, COLOR)
#define BROFILER_CATEGORY1(NAME, COLOR)
#define BROFILER_CATEGORY2(NAME, COLOR)
#define BROFILER_CATEGORY3(NAME, COLOR)
#define BROFILER_CATEGORY4(NAME, COLOR)
#define BROFILER_CATEGORY5(NAME, COLOR)
#elif BROFILER_COMPILED_LEVEL <= 1
#define BROFILER_EVENT_WITH_MASK2(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK3(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK4(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK5(MASK, NAME)
#define BROFILER_CATEGORY_WITH_MASK2(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK3(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK4(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK5(MASK, NAME, COLOR)
#define BROFILER_CATEGORY2(NAME, COLOR)
#define BROFILER_CATEGORY3(NAME, COLOR)
#define BROFILER_CATEGORY4(NAME, COLOR)
#define BROFILER_CATEGORY5(NAME, COLOR)
#elif BROFILER_COMPILED_LEVEL <= 2
#define BROFILER_EVENT_WITH_MASK3(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK4(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK5(MASK, NAME)
#define BROFILER_CATEGORY_WITH_MASK3(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK4(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK5(MASK, NAME, COLOR)
#define BROFILER_CATEGORY3(NAME, COLOR)
#define BROFILER_CATEGORY4(NAME, COLOR)
#define BROFILER_CATEGORY5(NAME, COLOR)
#elif BROFILER_COMPILED_LEVEL <= 3
#define BROFILER_EVENT_WITH_MASK4(MASK, NAME)
#define BROFILER_EVENT_WITH_MASK5(MASK, NAME)
#define BROFILER_CATEGORY_WITH_MASK4(MASK, NAME, COLOR)
#define BROFILER_CATEGORY_WITH_MASK5(MASK, NAME, COLOR)
#define BROFILER_CATEGORY4(NAME, COLOR)
#define BROFILER_CATEGORY5(NAME, COLOR)
#elif BROFILER_COMPILED_LEVEL <= 4
#define BROFILER_EVENT_WITH_MASK5(MASK, NAME)
#define BROFILER_CATEGORY_WITH_MASK5(MASK, NAME, COLOR)
#define BROFILER_CATEGORY5(NAME, COLOR)
#endif

// Perf counter
#define BROFILER_COUNTER_DECL(name, vmin, vmax)  Brofiler::CounterAPI::DeclCounter(#name, vmin, vmax);

#define BROFILER_COUNTER_SET(name, v) static uint8_t s_perfcounter_##name##_idx = Brofiler::CounterAPI::GetCounterIndex(#name); \
									Brofiler::CounterAPI::SetCounter(s_perfcounter_##name##_idx, (double)(v));

#define BROFILER_COUNTER_INC(name, v)   static uint8_t s_perfcounter_##name##_idx = Brofiler::CounterAPI::GetCounterIndex(#name); \
									Brofiler::CounterAPI::IncCounter(s_perfcounter_##name##_idx, (double)(v));

#define BROFILER_COUNTER_DEC(name, v)  BROFILER_COUNTER_INC(name, -v)

#define BROFILER_COUNTER_RESET(name)  BROFILER_COUNTER_SET(name, 0)

																				
#else
#define BROFILER_EVENT(NAME)
#define PROFILE
#define BROFILER_INLINE_EVENT(NAME, CODE) { CODE; }
#define BROFILER_CUSTOM_EVENT(DESCRIPTION)
#define BROFILER_CATEGORY(NAME, COLOR)
#define BROFILER_FRAME(NAME)
#define BROFILER_THREAD(FRAME_NAME)
#define BROFILER_START_THREAD(FRAME_NAME)
#define BROFILER_STOP_THREAD()
#define BROFILER_CATEGORY1(NAME, COLOR)
#define BROFILER_CATEGORY2(NAME, COLOR)
#define BROFILER_CATEGORY3(NAME, COLOR)
#define BROFILER_CATEGORY4(NAME, COLOR)
#define BROFILER_CATEGORY5(NAME, COLOR)
#define BROFILER_SET_CAPTURE_MASK(MASK)
#define BROFILER_EVENT_WITH_MASK(MASK, NAME)
#define BROFILER_CATEGORY_WITH_MASK(MASK, NAME, COLOR)
#endif
