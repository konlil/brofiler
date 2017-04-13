﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows;

namespace Profiler
{
    public struct NetworkProtocol
    {
/*
        public const UInt32 NETWORK_PROTOCOL_VERSION_6  = 6; 
        public const UInt32 NETWORK_PROTOCOL_VERSION_7  = 7; // Changed ThreadID - uint32 => uint64
        public const UInt32 NETWORK_PROTOCOL_VERSION_8  = 8; // Changed CoreID in SyncData - uint32 => uint64
        public const UInt32 NETWORK_PROTOCOL_VERSION_9  = 9; // Added thread synchronization wait reason
        public const UInt32 NETWORK_PROTOCOL_VERSION_10 = 10; // Added StackWalk event
		public const UInt32 NETWORK_PROTOCOL_VERSION_11 = 11; // Added thread synchronization switch to thread ID
 */ 
		public const UInt32 NETWORK_PROTOCOL_VERSION_12 = 12; // Added separate fiber sync data stream

		public const UInt32 NETWORK_PROTOCOL_VERSION = NETWORK_PROTOCOL_VERSION_12;
		public const UInt32 NETWORK_PROTOCOL_MIN_VERSION = NETWORK_PROTOCOL_VERSION_12;
    }

	public abstract class IResponseHolder
	{
		public abstract DataResponse Response { get; set; }
	}

    public class DataResponse
    {
        public enum Type
        {
            FrameDescriptionBoard = 0,
            EventFrame = 1,
            SamplingFrame = 2,
            Synchronization = 3,
            NullFrame = 4,
            ReportProgress = 5,
            Handshake = 6,
            SymbolPack = 7,
            CallstackPack = 8,
			SyscallPack = 9,
            ReportCounters = 10,
        }

        public Type ResponseType { get; set; }
        public UInt32 Version { get; set; }
        public BinaryReader Reader { get; set; }

        public DataResponse(Type type, UInt32 version, BinaryReader reader)
        {
            ResponseType = type;
            Version = version;
            Reader = reader;
        }

        public DataResponse(Type type, Stream stream)
        {
            ResponseType = type;
            Version = NetworkProtocol.NETWORK_PROTOCOL_VERSION;
            Reader = new BinaryReader(stream);
        }

        public String SerializeToBase64()
        {
            MemoryStream stream = new MemoryStream();
            Serialize(ResponseType, Reader.BaseStream, stream);
            stream.Position = 0;

            byte[] data = new byte[stream.Length];
            stream.Read(data, 0, (int)stream.Length);
            return Convert.ToBase64String(data);
        }

        public static void Serialize(DataResponse.Type type, Stream data, Stream result)
        {
            BinaryWriter writer = new BinaryWriter(result);
            writer.Write(NetworkProtocol.NETWORK_PROTOCOL_VERSION);
            writer.Write((UInt32)data.Length);
            writer.Write((UInt32)type);

            long position = data.Position;
            data.Seek(0, SeekOrigin.Begin);
            data.CopyTo(result);
            data.Seek(position, SeekOrigin.Begin);
        }

        public void Serialize(Stream result)
        {
            BinaryWriter writer = new BinaryWriter(result);
            writer.Write((UInt32)Version);
            writer.Write((UInt32)Reader.BaseStream.Length);
            writer.Write((UInt32)ResponseType);

            long position = Reader.BaseStream.Position;
            Reader.BaseStream.Seek(0, SeekOrigin.Begin);
            Reader.BaseStream.CopyTo(result);
            Reader.BaseStream.Seek(position, SeekOrigin.Begin);
        }

        public static DataResponse Create(Stream stream)
        {
            if (stream == null || !stream.CanRead)
                return null;

            var reader = new BinaryReader(stream);

            try
            {
                uint version = reader.ReadUInt32();
                uint length = reader.ReadUInt32();
                DataResponse.Type responseType = (DataResponse.Type)reader.ReadUInt32();
                byte[] bytes = reader.ReadBytes((int)length);

                return new DataResponse(responseType, version, new BinaryReader(new MemoryStream(bytes)));
            }
            catch (EndOfStreamException) { }

            return null;
        }

        public static DataResponse Create(String base64)
        {
            MemoryStream stream = new MemoryStream(Convert.FromBase64String(base64));
            return DataResponse.Create(stream);
        }
    }

    public enum CaptureType
    {
        Full,
        Events,
        Counters,
    }

    public enum MessageType
    {
        Start,
        Stop,
        GlobalCaptureMask,
        SetupHook,
    }

    public abstract class Message
    {
        public static UInt32 MESSAGE_MARK = 0xB50FB50F;

        public abstract Int32 GetMessageType();
        public virtual void Write(BinaryWriter writer)
        {
            writer.Write(GetMessageType());
        }
    }

    class StartMessage : Message
    {
        private int capture_type;
        private UInt32 capture_mask;
        private int capture_thresh;
        public StartMessage(int type, UInt32 mask, int thresh)
        {
            this.capture_mask = mask;
            this.capture_type = type;
            this.capture_thresh = thresh;
        }

        public override Int32 GetMessageType()
        {
            return (Int32)MessageType.Start;
        }

        public override void Write(BinaryWriter writer)
        {
            base.Write(writer);
            writer.Write(capture_type);
            writer.Write(capture_mask);
            writer.Write(capture_thresh);
        }
    }

    class StopMessage : Message
    {
        public override Int32 GetMessageType()
        {
            return (Int32)MessageType.Stop;
        }
    }

    class GlobalCaptureMaskMessage : Message
    {
        UInt32 capture_mask;

        public GlobalCaptureMaskMessage(UInt32 mask)
        {
            this.capture_mask = mask;
        }

        public override Int32 GetMessageType()
        {
            return (Int32)MessageType.GlobalCaptureMask;
        }

        public override void Write(BinaryWriter writer)
        {
            base.Write(writer);
            writer.Write(capture_mask);
        }
    }

    class SetupHookMessage : Message
    {
        UInt64 address;
        bool isHooked;

        public SetupHookMessage(UInt64 address, bool isHooked)
        {
            this.address = address;
            this.isHooked = isHooked;
        }

        public override Int32 GetMessageType()
        {
            return (Int32)MessageType.SetupHook;
        }

        public override void Write(BinaryWriter writer)
        {
            base.Write(writer);
            writer.Write(address);
            writer.Write(isHooked);
        }
    }

}
