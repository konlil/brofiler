#include "Common.h"
#include "Serialization.h"

namespace Brofiler
{
	std::string OutputDataStream::GetData()
	{
		flush();
		return str();
	}

	Brofiler::OutputDataStream OutputDataStream::Empty;

	OutputDataStream &operator << ( OutputDataStream &stream, const char* val )
	{
		uint32_t length = val == nullptr ? 0 : (uint32_t)strlen(val);
		stream << length;

		if (length > 0)
		{
			stream.write( val, length );
		}
		return stream;
	}

	OutputDataStream &operator << ( OutputDataStream &stream, int val )
	{
		stream.write( (char*)&val, sizeof(int) );
		return stream;
	}

	OutputDataStream &operator << ( OutputDataStream &stream, int64_t val )
	{
		stream.write( (char*)&val, sizeof(int64_t) );
		return stream;
	}

	OutputDataStream &operator << ( OutputDataStream &stream, char val )
	{
		stream.write( (char*)&val, sizeof(char) );
		return stream;
	}

	OutputDataStream &operator << (OutputDataStream &stream, int8_t val)
	{
		stream.write((char*)&val, sizeof(val));
		return stream;
	}

	OutputDataStream &operator << ( OutputDataStream &stream, byte val )
	{
		stream.write( (char*)&val, sizeof(byte) );
		return stream;
	}
	
	OutputDataStream & operator<<(OutputDataStream &stream, uint64_t val)
	{
		stream.write( (char*)&val, sizeof(uint64_t) );
		return stream;
	}

	OutputDataStream & operator<<(OutputDataStream &stream, uint32_t val)
	{
		stream.write( (char*)&val, sizeof(uint32_t) );
		return stream;
	}

	OutputDataStream & operator<<(OutputDataStream &stream, const std::string& val)
	{
		stream << (uint32_t)val.size();
		if (!val.empty())
			stream.write(&val[0], sizeof(val[0]) * val.size());
		return stream;
	}

	OutputDataStream & operator<<(OutputDataStream &stream, const std::wstring& val)
	{
		size_t count = val.size() * sizeof(wchar_t);
		stream << (uint32_t)count;
		if (!val.empty())
			stream.write((char*)(&val[0]), count);
		return stream;
	}

	OutputDataStream & operator<<(OutputDataStream &stream, const std::thread::id& val)
	{
		stream << val;
		return stream;
	}

	InputDataStream &operator >> ( InputDataStream &stream, int32_t &val )
	{
		stream.read( (char*)&val, sizeof(int) );
		return stream;
	}

	InputDataStream &operator >> ( InputDataStream &stream, int64_t &val )
	{
		stream.read( (char*)&val, sizeof(int64_t) );
		return stream;
	}

	InputDataStream & operator>>( InputDataStream &stream, byte &val )
	{
		stream.read( (char*)&val, sizeof(byte) );
		return stream;
	}

	InputDataStream & operator>>( InputDataStream &stream, uint32_t &val )
	{
		stream.read( (char*)&val, sizeof(uint32_t) );
		return stream;
	}

	InputDataStream & operator>>( InputDataStream &stream, uint64_t &val )
	{
		stream.read( (char*)&val, sizeof(uint64_t) );
		return stream;
	}

	InputDataStream::InputDataStream() :
		std::stringstream( ios_base::in | ios_base::out )
	{
	}

	void InputDataStream::Append(const char *buffer, size_t length)
	{
		write( buffer, length );
	}

	size_t InputDataStream::Length()
	{
		return (size_t)(tellp() - tellg());
	}

	bool InputDataStream::Skip(size_t length)
	{
		bool result = Length() <= length;
		seekg(length, ios_base::cur);
		return result;
	}



}
