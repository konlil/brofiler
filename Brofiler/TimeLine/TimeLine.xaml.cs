using System;
using System.Collections.Generic;
using System.Text;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Navigation;
using System.Windows.Shapes;
using System.Collections.ObjectModel;
using System.Net.Sockets;
using System.Threading;
using System.Net;
using System.IO;
using System.Windows.Threading;
using Profiler.Data;
using Frame = Profiler.Data.Frame;
using Microsoft.Win32;
using System.Xml;
using System.Net.Cache;
using System.Reflection;
using System.Diagnostics;
using System.Web;
using System.Net.NetworkInformation;
using System.Text.RegularExpressions;

namespace Profiler
{
    public delegate void ClearAllFramesHandler();

    /// <summary>
    /// Interaction logic for TimeLine.xaml
    /// </summary>
    public partial class TimeLine : UserControl
    {
        FrameCollection frames = new FrameCollection();
        Thread socketThread = null;

        Object criticalSection = new Object();

		public FrameCollection Frames
		{
			get
			{
				return frames;
			}
		}

        public TimeLine()
        {
            this.InitializeComponent();
            this.DataContext = frames;

            this.Loaded += new RoutedEventHandler(TimeLine_Loaded);

            System.Net.IPAddress defaultIP;
            System.Net.IPAddress.TryParse(Properties.Settings.Default.DefaultIP, out defaultIP);
            ProfilerClient.Get().IpAddress = defaultIP;
            ProfilerClient.Get().Port = Properties.Settings.Default.DefaultPort;
        }

        Version CurrentVersion { get { return Assembly.GetExecutingAssembly().GetName().Version; } }

        void TimeLine_Loaded(object sender, RoutedEventArgs e)
        {
        }

        private void Hyperlink_RequestNavigate(object sender, RequestNavigateEventArgs e)
        {
            Process.Start(new ProcessStartInfo(e.Uri.AbsoluteUri));
            e.Handled = true;
        }

        private bool Open(Stream stream)
        {
            DataResponse response = DataResponse.Create(stream);
            while (response != null)
            {
                if (!ApplyResponse(response))
                    return false;

                response = DataResponse.Create(stream);
            }

            frames.Flush();
            ScrollToEnd();

            return true;
        }

        Dictionary<DataResponse.Type, int> testResponses = new Dictionary<DataResponse.Type, int>();

        private void SaveTestResponse(DataResponse response)
        {
            if (!testResponses.ContainsKey(response.ResponseType))
                testResponses.Add(response.ResponseType, 0);

            int count = testResponses[response.ResponseType]++;

            String data = response.SerializeToBase64();
            String path = response.ResponseType.ToString() + "_" + String.Format("{0:000}", count) + ".bin";
            File.WriteAllText(path, data);

        }

        public class ThreadDescription
        {
            public UInt32 ThreadID { get; set; }
            public String Name { get; set; }

            public override string ToString()
            {
                return String.Format("[{0}] {1}", ThreadID, Name);
            }
        }

        private bool ApplyResponse(DataResponse response)
        {
            if (response.Version >= NetworkProtocol.NETWORK_PROTOCOL_MIN_VERSION)
            {
                //SaveTestResponse(response);

                switch (response.ResponseType)
                {
                    case DataResponse.Type.ReportProgress:
                        Int32 length = response.Reader.ReadInt32();
                        StatusText.Text = new String(response.Reader.ReadChars(length));
                        break;

                    case DataResponse.Type.ReportCounters:
                        {
                            MainWindow w = (MainWindow)(Window.GetWindow(this));
                            w.HandleCounterMessage(response);
                        }
                        break;

                    case DataResponse.Type.NullFrame:
						StatusText.Visibility = System.Windows.Visibility.Collapsed;
                        lock (frames)
                        {
                            frames.Flush();
                            ScrollToEnd();
                        }
                        break;

                    case DataResponse.Type.Handshake:
                        break;

                    default:
                        lock (frames)
                        {
                            frames.Add(response);
                            //ScrollToEnd();
                        }
                        break;
                }
            }
            else
            {
                MessageBox.Show("Invalid NETWORK_PROTOCOL_VERSION");
                return false;
            }
            return true;
        }

        private void ScrollToEnd()
        {
            if (frames.Count > 0)
            {
                frameList.SelectedItem = frames[frames.Count - 1];
                frameList.ScrollIntoView(frames[frames.Count - 1]);
            }
        }

        public void RecieveMessage()
        {
            while (true)
            {
                DataResponse response = ProfilerClient.Get().RecieveMessage();

                if (response != null)
                    Application.Current.Dispatcher.BeginInvoke(new Action(() => ApplyResponse(response)));
                else
                    Thread.Sleep(1000);
            }
        }

        #region FocusFrame
        private void FocusOnFrame(Data.Frame frame)
        {
            FocusFrameEventArgs args = new FocusFrameEventArgs(FocusFrameEvent, frame);
            RaiseEvent(args);
        }

        public void ScrollToItem(Data.Frame frame)
        {
            for (int i = 0; i < frames.Count; i++)
            {
                if (frames[i] == frame)
                {
                    frameList.SelectedItem = frames[i];
                    frameList.ScrollIntoView(frames[i]);
                }
            }
        }

        public class FocusFrameEventArgs : RoutedEventArgs
        {
            public Data.Frame Frame { get; set; }
            public Data.EventNode Node { get; set; }
			public ITick Tick { get; set; }

            public FocusFrameEventArgs(RoutedEvent routedEvent, Data.Frame frame, Data.EventNode node = null, ITick tick = null)
                : base(routedEvent)
            {
                Frame = frame;
                Node = node;
				Tick = tick;
            }
        }

        public delegate void FocusFrameEventHandler(object sender, FocusFrameEventArgs e);

        public ClearAllFramesHandler OnClearAllFrames;

        public static readonly RoutedEvent FocusFrameEvent = EventManager.RegisterRoutedEvent("FocusFrame", RoutingStrategy.Bubble, typeof(FocusFrameEventHandler), typeof(TimeLine));

        public event RoutedEventHandler FocusFrame
        {
            add { AddHandler(FocusFrameEvent, value); }
            remove { RemoveHandler(FocusFrameEvent, value); }
        }
        #endregion

        private void frameList_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            if (frameList.SelectedItem is Data.Frame)
            {
                FocusOnFrame((Data.Frame)frameList.SelectedItem);
            }
        }

        public void Close()
        {
            if (socketThread != null)
            {
                socketThread.Abort();
                socketThread = null;
            }
        }

        private void SafeCopy(Stream from, Stream to)
        {
            long pos = from.Position;
            from.Seek(0, SeekOrigin.Begin);
            from.CopyTo(to);
            from.Seek(pos, SeekOrigin.Begin);
        }

        private void OpenButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            OpenFileDialog dlg = new OpenFileDialog();
            dlg.Filter = "Brofiler files (*.prof)|*.prof";
            dlg.Title = "Load profiler results?";
            if (dlg.ShowDialog() == true)
            {
                LoadFile(dlg.FileName);
            }

        }

        public void LoadFile(string file)
        {
            if (File.Exists(file))
            {
				using (new WaitCursor())
				{
					using (FileStream stream = new FileStream(file, FileMode.Open))
					{
						Open(stream);
					}
				}
            }
        }


        private void SaveButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            SaveFileDialog dlg = new SaveFileDialog();
            dlg.Filter = "Brofiler files (*.prof)|*.prof";
            dlg.Title = "Where should I save profiler results?";

            if (dlg.ShowDialog() == true)
            {
                lock (frames)
                {
                    FileStream stream = new FileStream(dlg.FileName, FileMode.Create);

                    HashSet<EventDescriptionBoard> boards = new HashSet<EventDescriptionBoard>();
                    HashSet<FrameGroup> groups = new HashSet<FrameGroup>();

					FrameGroup currentGroup = null;

                    foreach (Frame frame in frames)
                    {
						if (frame is EventFrame)
						{
							EventFrame eventFrame = frame as EventFrame;
							if (eventFrame.Group != currentGroup && currentGroup != null)
							{
								currentGroup.Responses.ForEach(response => response.Serialize(stream));
							}
							currentGroup = eventFrame.Group;
						}
						else if (frame is SamplingFrame)
						{
							if (currentGroup != null)
							{
								currentGroup.Responses.ForEach(response => response.Serialize(stream));
								currentGroup = null;
							}

							(frame as SamplingFrame).Response.Serialize(stream);
						}
                    }

					if (currentGroup != null)
					{
						currentGroup.Responses.ForEach(response => 
						{
							response.Serialize(stream);
						});
					}

                    stream.Close();
                }
            }
        }

        private void ClearButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            lock (frames)
            {
                frames.Clear();
            }

            OnClearAllFrames();
        }

        private static bool ConvertText2Hex(string text, out UInt32 result)
        {
            result = 0;
            if (text == "" || text == "0" || text == "0x" || text == "0X")
            {
                return true;
            }

            if (text.Length > 2 && (text.StartsWith("0x") || text.StartsWith("0X") ))
            {
                text = text.Substring(2);
                if (text.Length > 8) return false;

                bool success = UInt32.TryParse(text, System.Globalization.NumberStyles.HexNumber, new System.Globalization.CultureInfo("en-US"), out result);
                return success;
            }

            return false;
        }
        
        private string MaskText_LastValid = "";
        private void MaskText_TextChanged(object sender, System.Windows.Controls.TextChangedEventArgs e)
        {
            UInt32 mask = 0;
            if (!ConvertText2Hex(MaskText.Text, out mask))
            {
                MaskText.Text = MaskText_LastValid;
                MessageBox.Show("Invalid input, must be hex style of uint32 value");
            }
            else
            {
                MaskText_LastValid = MaskText.Text;
            }
        }

        private int capture_filter_thresh = 0;

        private void GlobalCaptureMaskButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            UInt32 old_mask = 0;
            ConvertText2Hex(MaskText.Text, out old_mask);

            Profiler.MaskSetting settingDlg = new Profiler.MaskSetting();
            settingDlg.Owner = Application.Current.MainWindow; // We must also set the owner for this to work.
            settingDlg.WindowStartupLocation = WindowStartupLocation.Manual;
            settingDlg.Top = Application.Current.MainWindow.Top + 60;
            settingDlg.Left = Application.Current.MainWindow.Left + 200;
            settingDlg.MaskValue = old_mask;
            settingDlg.FilterThresh = capture_filter_thresh;
            settingDlg.ShowDialog();
            Console.WriteLine("mask: {0}", settingDlg.MaskValue);
            MaskText.Text = String.Format("0x{0:X8}", settingDlg.MaskValue);
            capture_filter_thresh = settingDlg.FilterThresh;
        }

        private void ClearHooksButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            //ProfilerClient.Get().SendMessage(new SetupHookMessage(0, false));
        }

        private void StopButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            ProfilerClient.Get().SendMessage(new StopMessage());
            StartButton.IsEnabled = true;
            StopButton.IsEnabled = false;
        }

        private UInt32 GetCurrentCaptureMask()
        {
            UInt32 mask = 0;
            if (ConvertText2Hex(MaskText.Text, out mask))
                return mask;
            return 0;
        }

        private void StartButton_Click(object sender, System.Windows.RoutedEventArgs e)
        {
            var platform = PlatformCombo.ActivePlatform;

            if (platform == null)
                return;

            Properties.Settings.Default.DefaultIP = platform.IP.ToString();
            Properties.Settings.Default.DefaultPort = platform.Port;
            Properties.Settings.Default.Save();

            ProfilerClient.Get().IpAddress = platform.IP;
            ProfilerClient.Get().Port = platform.Port;

            int capture_type = StartComboBox.SelectedIndex;
            UInt32 capture_mask = GetCurrentCaptureMask();

            StartMessage message = new StartMessage(capture_type, capture_mask, capture_filter_thresh);
            if (ProfilerClient.Get().SendMessage(message))
            {
                Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                {
                    StatusText.Text = "Capturing...";
                    StatusText.Visibility = System.Windows.Visibility.Visible;
                }));

                if (socketThread == null)
                {
                    socketThread = new Thread(RecieveMessage);
                    socketThread.Start();
                }
            }
            else
            {
                MessageBox.Show("Send start message failed, check network setting pls", "Error", MessageBoxButton.OK, MessageBoxImage.Error);
                return;
            }
            
            StartButton.IsEnabled = false;
            StopButton.IsEnabled = true;
        }

        private DurationFilter duration_filter;
        //{
        //    get;
        //    set{ DurationFilterText.Text = string.Format("{0}:{1}", value.floor, value.ceil); }
        //}

        private bool CheckFilterInput(string text)
        {
            if (!text.Contains(":")) return false;
            var strs = text.Split(':');
            if (strs.Length != 2) return false;

            if (strs[0] == "" || strs[1] == "") return true;

            try
            {
                double floor = Convert.ToDouble(strs[0]);
                double ceil = Convert.ToDouble(strs[1]);
            }
            catch
            {
                return false;
            }
            return true;
        }

        private DurationFilter ParseDurationFilter(string text)
        {
            DurationFilter filter = new DurationFilter();
            var strs = text.Split(':');
            if (strs[0] == "")
                filter.floor = 0;
            else
                filter.floor = Convert.ToDouble(strs[0]);

            if (strs[1] == "")
                filter.ceil = double.MaxValue;
            else
                filter.ceil = Convert.ToDouble(strs[1]);
            return filter;
        }

        private void NextFilteredFrame()
        {
            if (frameList.SelectedIndex == frames.Count - 1)
                return;

            for (int i = frameList.SelectedIndex + 1; i < frames.Count; i++)
            {
                if ( frames[i].Duration >= duration_filter.floor && frames[i].Duration < duration_filter.ceil)
                {
                    frameList.SelectedItem = frames[i];
                    frameList.ScrollIntoView(frames[i]);
                    break;
                }
            }
        }

        private void PrevFilteredFrame()
        {
            if (frameList.SelectedIndex == 0)
                return;

            for (int i = frameList.SelectedIndex - 1; i >= 0; i--)
            {
                if (frames[i].Duration >= duration_filter.floor && frames[i].Duration < duration_filter.ceil)
                {
                    frameList.SelectedItem = frames[i];
                    frameList.ScrollIntoView(frames[i]);
                    break;
                }
            }
        }

        private void PrevFrame_Click(object sender, RoutedEventArgs e)
        {
            if (!CheckFilterInput(DurationFilterText.Text))
            {
                MessageBox.Show("Invalid filter text, should be {double}:{double}");
                return;
            }
            duration_filter = ParseDurationFilter(DurationFilterText.Text);
            //Console.WriteLine("filter: {0}, {1}", duration_filter.floor, duration_filter.ceil);
            PrevFilteredFrame();
        }

        private void NextFrame_Click(object sender, RoutedEventArgs e)
        {
            if (!CheckFilterInput(DurationFilterText.Text))
            {
                MessageBox.Show("Invalid filter text, should be {%f}:{%f}");
                return;
            }
            duration_filter = ParseDurationFilter(DurationFilterText.Text);
            //Console.WriteLine("filter: {0}, {1}", duration_filter.floor, duration_filter.ceil);
            NextFilteredFrame();
        }

        private void DurationFilterText_TextChanged(object sender, TextChangedEventArgs e)
        {

        }
    }

    public class DurationFilter
    {
        public double floor { get; set; }
        public double ceil { get; set; }
    }

    public class FrameHeightConverter : IValueConverter
    {
        public static double Convert(double value)
        {
            return 2.0 * value;
        }

        public object Convert(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return Convert((double)value);
        }

        public object ConvertBack(object value, Type targetType, object parameter, System.Globalization.CultureInfo culture)
        {
            return null;
        }
    }


	public class WaitCursor : IDisposable
	{
		private Cursor _previousCursor;

		public WaitCursor()
		{
			_previousCursor = Mouse.OverrideCursor;

			Mouse.OverrideCursor = Cursors.Wait;
		}

		public void Dispose()
		{
			Mouse.OverrideCursor = _previousCursor;
		}
	}
}