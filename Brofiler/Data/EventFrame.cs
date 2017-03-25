﻿using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Windows.Media;
using System.Threading;
using System.ComponentModel;

namespace Profiler.Data
{
    public class FrameHeader : EventData
    {
        public int ThreadIndex { get; private set; }

        public static FrameHeader Read(BinaryReader reader)
        {
            FrameHeader header = new FrameHeader();
            header.ThreadIndex = reader.ReadInt32();

            header.ReadEventData(reader);
            return header;
        }

        public FrameHeader() { }

        public FrameHeader(int threadIndex, IDurable duration)
        {
			ThreadIndex = threadIndex;
            Start = duration.Start;
            Finish = duration.Finish;
        }
    }


    public class ShortBoard : Dictionary<EventDescription, List<Entry>>
    {
        public ShortBoard(List<Entry> entries)
        {
            for (int i = 0; i < entries.Count; ++i)
            {
                Entry entry = entries[i];

                List<Entry> sharedEntries = null;
                if (!TryGetValue(entry.Description, out sharedEntries))
                {
                    sharedEntries = new List<Entry>();
                    Add(entry.Description, sharedEntries);
                }
                sharedEntries.Add(entry);
            }
        }

        public List<Entry> Get(EventDescription description)
        {
            List<Entry> result = null;
            TryGetValue(description, out result);
            return result;
        }
    }


    public class EventFrame : Frame, IDurable, IComparable<EventFrame>, INotifyPropertyChanged
    {
        public override DataResponse.Type ResponseType { get { return DataResponse.Type.EventFrame; } }

        public FrameHeader Header { get; private set; }
        public long Tick { get { return Header.Start; } }

        private List<Entry> entries;

        private EventTree root = null;
        public Profiler.Data.EventTree Root
        {
            get
            {
                if (root == null)
                    Load();

                return root;
            }
        }

        private BinaryReader reader = null;

        public long Start { get { return Header.Start; } }
        public long Finish { get { return Header.Finish; } }

		string filteredDescription = "";

        public override double Duration
        {
            get
            {
                return Header.Duration;
            }
        }

        public double SynchronizationDuration { get; set; }

        public override string Description
        {
            get
            {
                return String.Format("{0:0} ms", Header.Duration);
            }
        }

		public override string FilteredDescription
		{
			get
			{
				return filteredDescription;
			}

			set
			{
				filteredDescription = value;
				OnPropertyChanged("FilteredDescription"); 
			}
		}

		public event PropertyChangedEventHandler PropertyChanged;

		private void OnPropertyChanged(string propertyName)
		{
			if (PropertyChanged != null)
				PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
		}


        public string DeatiledDescription
        {
            get
            {
                return String.Format("Work: {0:0.###}ms   Wait: {1:0.###}ms", Duration - SynchronizationDuration, SynchronizationDuration).Replace(',', '.');
            }
        }

        public EventDescriptionBoard DescriptionBoard { get { return Group.Board; } }
        public FrameGroup Group { get; private set; }

        private Board<EventBoardItem, EventDescription, EventNode> board;
        public Board<EventBoardItem, EventDescription, EventNode> Board
        {
            get
            {
                if (board == null)
                    Load();

                return board;
            }
        }

        private ShortBoard shortBoard;
        public ShortBoard ShortBoard
        {
            get
            {
                Load();

                if (shortBoard == null)
                    shortBoard = new ShortBoard(entries);

                return shortBoard;
            }
        }

        public List<Entry> Entries
        {
            get
            {
                if (entries == null)
                    Load();

                return entries;
            }
        }

        public List<Entry> Categories { get; private set; }
        public EventTree CategoriesTree { get; private set; }
        public List<Data.SyncInterval> Synchronization { get; private set; }

        long IDurable.Finish
        {
            get
            {
                return Header.Finish;
            }

        }

        long ITick.Start
        {
            get
            {
                return Header.Start;
            }
        }

        Object loading = new object();

        public override void Load()
        {
            lock (loading)
            {
                if (!IsLoaded)
                {
                    entries = ReadEventList(reader, DescriptionBoard);

                    root = new EventTree(this, entries);
                    board = new Board<EventBoardItem, EventDescription, EventNode>(root);

                    reader = null;
                    IsLoaded = true;
                }
            }
        }

        static List<EventData> ReadEventTimeList(BinaryReader reader)
        {
            int count = reader.ReadInt32();
            List<EventData> result = new List<EventData>(count);

            for (int i = 0; i < count; ++i)
                result.Add(EventData.Create(reader));

            return result;
        }

        static List<Entry> ReadEventList(BinaryReader reader, EventDescriptionBoard board)
        {
            int count = reader.ReadInt32();
            List<Entry> result = new List<Entry>(count);

            for (int i = 0; i < count; ++i)
                result.Add(Entry.Read(reader, board));

            return result;
        }

        List<EventData> LinearizeEventList(List<EventData> events)
        {
            List<EventData> result = new List<EventData>(events.Count);
            EventData currentRoot = null;

            foreach (EventData entry in events)
            {
                if (currentRoot == null)
                {
                    currentRoot = entry;
                    result.Add(entry);
                }
                else if (entry.Finish <= currentRoot.Finish)
                {
                    continue;
                }
                else if (Durable.TicksToMs(Math.Abs(entry.Start - currentRoot.Finish)) < 0.005)
                {
                    currentRoot.Finish = entry.Finish;
                }
                else
                {
                    currentRoot = entry;
                    result.Add(entry);
                }
            }

            return result;
        }

        public void MergeWith(EventFrame frame)
        {
            Categories.AddRange(frame.Categories);
            Categories.Sort();

            Entries.AddRange(frame.Entries);
            Entries.Sort();

            root = new EventTree(this, Entries);
            board = new Board<EventBoardItem, EventDescription, EventNode>(root);

            CategoriesTree = new EventTree(this, Categories);
        }

        protected void ReadInternal(BinaryReader reader)
        {
            Header = FrameHeader.Read(reader);
            //Console.WriteLine("Recv Frame, duration: {0} ms, {1}, {2}", Header.Duration, Header.Start, Header.Finish);
            Categories = ReadEventList(reader, DescriptionBoard);
            CategoriesTree = new EventTree(this, Categories);

            Synchronization = new List<SyncInterval>();
        }

        public double CalculateFilteredTime(HashSet<Object> filter)
        {
            return Root.CalculateFilteredTime(filter);
        }

        public int CompareTo(EventFrame other)
        {
            return Start.CompareTo(other.Start);
        }

        public EventFrame(DataResponse response, FrameGroup group) : base(response)
        {
            reader = response.Reader;
            Group = group;
            ReadInternal(reader);
        }

        public EventFrame(FrameHeader header, List<Entry> entries, FrameGroup group) : base(null)
        {
            Header = header;
            Group = group;
            Categories = new List<Entry>();

            this.entries = entries;
			foreach (Entry entry in entries)
			{
				if (entry.Description.Color.A != 0)
				{
					Categories.Add(entry);
				}
			}

            CategoriesTree = new EventTree(this, Categories);

            root = new EventTree(this, Entries);
            board = new Board<EventBoardItem, EventDescription, EventNode>(root);

            Synchronization = new List<SyncInterval>();

            IsLoaded = true;
        }
    }
}
