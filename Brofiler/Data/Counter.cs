using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.IO;
using System.Threading.Tasks;
using System.Collections.ObjectModel;
using System.ComponentModel;
using Microsoft.Research.DynamicDataDisplay.Common;

namespace Profiler.Data
{
    public class CounterCollection
    {
        Dictionary<uint, CounterDeclaration> counter_ids = new Dictionary<uint, CounterDeclaration>();
        Dictionary<uint, CounterPointArray> counters = new Dictionary<uint, CounterPointArray>();

        public ObservableCollection<CounterDeclaration> CounterDecls = new ObservableCollection<CounterDeclaration>();
        public SortedSet<uint> AlarmedCounters = new SortedSet<uint>();

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
                    double vmin = reader.ReadDouble();
                    double vmax = reader.ReadDouble();
                    uint idx = reader.ReadUInt32();

                    var decl = new CounterDeclaration(name, vmin, vmax, idx);
                    counter_ids[idx] = decl;

                    CounterDecls.Add(decl);
                }
            }

            uint size = reader.ReadUInt32();
            for (uint i = 0; i < size; i++)
            {
                if (!counters.ContainsKey(i))
                    counters[i] = new CounterPointArray();

                CounterPointArray arr = counters[i];
                double v = reader.ReadDouble();

                var decl = counter_ids[i];
                if (v < decl.min_value || v > decl.max_value)
                {
                    decl.alarm_count += 1;
                    AlarmedCounters.Add(i);
                }

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

    public class CounterDeclaration : INotifyPropertyChanged
    {
        public CounterDeclaration(string n, double vmin, double vmax, uint i)
        {
            this.name = n;
            this.min_value = vmin;
            this.max_value = vmax;
            this.id = i;
            this.alarm_count = 0;
        }

        private uint _alarm_count = 0;

        public string name { get; set; }
        public double min_value { get; set; }
        public double max_value { get; set; }
        public uint id { get; set; }
        public uint alarm_count 
        {
            get { return _alarm_count; }
            set
            {
                if (_alarm_count != value)
                {
                    _alarm_count = value;
                    NotifyPropertyChanged("alarm_count");
                }
            }
        }

        #region INotifyPropertyChanged Members

        /// Need to implement this interface in order to get data binding
        /// to work properly.
        private void NotifyPropertyChanged(string propertyName)
        {
            if (PropertyChanged != null)
            {
                PropertyChanged(this, new PropertyChangedEventArgs(propertyName));
            }
        }

        public event PropertyChangedEventHandler PropertyChanged;

        #endregion
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
