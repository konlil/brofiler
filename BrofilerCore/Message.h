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
		TurnSampling,
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
struct StartMessage : public Message<IMessage::Start>
{
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
struct TurnSamplingMessage : public Message<IMessage::TurnSampling>
{
	int32_t index;
	byte isSampling;

	static IMessage* Create(InputDataStream& stream);
	virtual void Apply() override;
};
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
}
