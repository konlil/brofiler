using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using Microsoft.Research.DynamicDataDisplay.Common;

namespace Profiler.Data
{
    public class CounterCollection
    {
        Dictionary<string, uint> counter_ids = new Dictionary<string, uint>();
        Dictionary<uint, CounterPointArray> counters = new Dictionary<uint, CounterPointArray>();

        public ObservableCollection<CounterNameIDPair> CounterNames = new ObservableCollection<CounterNameIDPair>();

        public void Read(DataResponse response)
        {
            BinaryReader reader = response.Reader;

            UInt64 frame_id = reader.ReadUInt64();

            byte has_new_counter = reader.ReadByte();
            if (has_new_counter > 0)
            {
                int new_counter_cnt = reader.ReadInt32();
                for (int i = 0; i < new_counter_cnt; i++)
                {
                    int name_len = reader.ReadInt32();
                    string name = new string(reader.ReadChars(name_len));
                    uint idx = reader.ReadUInt32();
                    counter_ids[name] = idx;

                    CounterNames.Add(new CounterNameIDPair(name, idx));
                }
            }

            uint size = reader.ReadUInt32();
            for (uint i = 0; i < size; i++)
            {
                if (!counters.ContainsKey(i))
                    counters[i] = new CounterPointArray();

                CounterPointArray arr = counters[i];
                double v = reader.ReadDouble();

                var pt = new CounterPoint(frame_id, v);
                arr.Add(pt);
            }
        }

        public CounterPointArray GetCounterArray(uint idx)
        {
            if (!counters.ContainsKey(idx))
                counters[idx] = new CounterPointArray();
            return counters[idx];
        }
    }

    public class CounterPoint
    {
        public UInt64 Id { get; set; }
        public double Count { get; set; }

        public CounterPoint(UInt64 id, double cnt)
        {
            this.Id = id;
            this.Count = cnt;
        }
    }

    public class CounterPointArray : RingArray <CounterPoint>
    {
        private const int MAX_POINTS = 1024;
        public CounterPointArray()
            : base(MAX_POINTS)
        {
        }
    }

    public class CounterNameIDPair
    {
        public CounterNameIDPair(string n, uint i)
        {
            this.name = n;
            this.id = i;
        }
        public string name { get; set; }
        public uint id { get; set; }
    }

    //public class CounterValue
    //{
    //    public string name { get; set; }
    //    public double value { get; set; }
    //}

    //public class CounterFrame
    //{
    //    Dictionary<string, uint> counter_ids = new Dictionary<string,uint>();
    //    List<double> counters = new List<double>();

    //    public static CounterFrame Read(DataResponse response)
    //    {
    //        BinaryReader reader = response.Reader;

    //        UInt64 frame_id = reader.ReadUInt64();

    //        CounterFrame frame = new CounterFrame();
    //        byte has_new_counter = reader.ReadByte();
    //        if (has_new_counter > 0)
    //        {
    //            int new_counter_cnt = reader.ReadInt32();
    //            for (int i = 0; i < new_counter_cnt; i++)
    //            {
    //                int name_len = reader.ReadInt32();
    //                string name = new string(reader.ReadChars(name_len));
    //                uint idx = reader.ReadUInt32();
    //                frame.counter_ids[name] = idx;
    //            }
    //        }

    //        uint size = reader.ReadUInt32();
    //        for (int i = 0; i < size; i++)
    //        {
    //            double v = reader.ReadDouble();
    //            frame.counters.Add(v);
    //        }
    //        return frame;
    //    }
    //}
}
