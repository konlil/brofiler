#pragma once
#include "Common.h"
#include "Serialization.h"

namespace Brofiler
{
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static const uint32_t NETWORK_PROTOCOL_VERSION = 12;
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct DataResponse
{
	enum Type
	{
		FrameDescriptionBoard = 0,			// DescriptionBoard for Instrumental Frames
		EventFrame = 1,						// Instrumental Data
		SamplingFrame = 2,					// Sampling Data
		Synchronization = 3,				// SwitchContext Data
		NullFrame = 4,						// Last Fame Mark
		ReportProgress = 5,					// Report Current Progress
		Handshake = 6,						// Handshake Response
		SymbolPack = 7,						// A pack full of resolved Symbols
		CallstackPack = 8,					// Callstack Pack
		SyscallPack = 9,					// SysCalls Pack
		ReportCounters = 10,				// Report Counters
	};

	uint32_t version;
	uint32_t size;
	Type type;

	DataResponse(Type t, uint32_t s) : version(NETWORK_PROTOCOL_VERSION), size(s), type(t) {}
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
OutputDataStream& operator << (OutputDataStream& os, const DataResponse& val);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
class IMessage
{
public:
	enum Type
	{
		Start,
		Stop,
		GlobalCaptureMask,
		COUNT,
	};

	virtual void Apply() = 0;
	virtual ~IMessage() {}

	static IMessage* Create( InputDataStream& str );
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
template<IMessage::Type MESSAGE_TYPE>
class Message : public IMessage
{
	enum { id = MESSAGE_TYPE };
public:
	static uint32_t GetMessageType() { return id; }
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#define CAPTURE_ALL 0
#define CAPTURE_ONLY_EVENTS 1
#define CAPTURE_ONLY_COUNTERS 2
struct StartMessage : public Message<IMessage::Start>
{
	int capture_type;
	uint32_t capture_mask;
	static IMessage* Create(InputDataStream&);
	virtual void Apply() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct StopMessage : public Message<IMessage::Stop>
{
	static IMessage* Create(InputDataStream&);
	virtual void Apply() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
struct GlobalCaptureMaskMessage : public Message<IMessage::GlobalCaptureMask>
{
	uint32_t mask_value;
	static IMessage* Create(InputDataStream&);
	virtual void Apply() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
