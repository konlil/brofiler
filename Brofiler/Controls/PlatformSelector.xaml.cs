﻿using System;
using System.Collections.Generic;
using System.Windows;
using System.Windows.Controls;

using System.Net;
using System.ComponentModel;
using System.Collections.ObjectModel;
using System.Net.NetworkInformation;
using System.Net.Sockets;
using System.Threading.Tasks;

namespace Profiler
{
    /// <summary>
    /// Interaction logic for PlatformSelector.xaml
    /// </summary>
    public partial class PlatformSelector : UserControl
    {
        public class PlatformDescription : INotifyPropertyChanged
        {
            public String Name { get; set; }

            IPAddress ip = IPAddress.None;
            public IPAddress IP
            {
                get
                {
                    return ip;
                }
                set
                {
                    ip = value;
                    if (PropertyChanged != null)
                        PropertyChanged(this, new PropertyChangedEventArgs("Status"));
                }
            }  

            public short Port { get; set; }

            public String Icon { get; set; }

            public bool Detailed { get; set; }

            public String Status
            {
                get
                {
                    return IP.Equals(IPAddress.None) ? "Disconnected" : IP.ToString();
                }
            }

            public const short DEFAULT_PORT = 31313;

            public event PropertyChangedEventHandler PropertyChanged;

            public PlatformDescription()
            {
                Port = DEFAULT_PORT;
            }
        }

        public PlatformDescription ActivePlatform
        {
            get
            {
                return comboBox.SelectedItem as PlatformDescription;
            }
        }

        ObservableCollection<PlatformDescription> platforms = new ObservableCollection<PlatformDescription>();

        public PlatformSelector()
        {
            InitializeComponent();

            IPAddress ip = Platform.GetPCAddress();

            int defaultSelectionIndex = 0;
            platforms.Add(new PlatformDescription() { Name = Environment.MachineName, IP = ip, Icon = "appbar_os_windows_8" });

            platforms.Add(new PlatformDescription() { Name = "PS4", IP = Platform.GetPS4Address(),  Icon = "appbar_social_playstation" });
            platforms.Add(new PlatformDescription() { Name = "Xbox", IP = Platform.GetXONEAddress(), Icon = "appbar_controller_xbox" });
            platforms.Add(new PlatformDescription() { Name = "Network", IP = IPAddress.Loopback, Icon = "appbar_network", Detailed = true });
            platforms.Add(new PlatformDescription() { Name = "IPhone", IP = Platform.GetIPhoneAddress(), Icon = "appbar_network", Detailed = true });

            comboBox.ItemsSource = platforms;

            comboBox.SelectedIndex = defaultSelectionIndex;

/*
            IPAddress savedIP = ip;
            short savedPort = Properties.Settings.Default.DefaultPort;
            IPAddress.TryParse(Properties.Settings.Default.DefaultIP, out savedIP);

            AddPlatform(savedIP, savedPort, true);


            ScanNetworkForCompatibleDevices(ip);
*/
        }

        private String GetIconByComputerName(String name)
        {
            String result = "appbar_os_windows_8";

            if (name.IndexOf("xbox", StringComparison.OrdinalIgnoreCase) != -1)
                result = "appbar_controller_xbox";
            else if (name.IndexOf("ps4", StringComparison.OrdinalIgnoreCase) != -1)
                result = "appbar_social_playstation";

            return result;
        }

/*
        private void AddPlatform(IPAddress ip, short port, bool autofocus)
        {
            if (ip.Equals(IPAddress.None) || ip.Equals(IPAddress.Any) || ip.Equals(IPAddress.Loopback))
                return;

            Task.Run(() =>
            {
                PingReply reply = new Ping().Send(ip, 16);

                if (reply.Status == IPStatus.Success)
                {
                    String name = reply.Address.ToString();

                    try
                    {
                        IPHostEntry entry = Dns.GetHostEntry(reply.Address);
                        if (entry != null)
                            name = entry.HostName;
                    }
                    catch (SocketException) { }

                    Application.Current.Dispatcher.BeginInvoke(new Action(() =>
                    {
                        var newPlatform = new PlatformDescription() { Name = name, IP = reply.Address, Port = port, Icon = GetIconByComputerName(name) };

                        bool needAdd = true;

                        foreach (PlatformDescription platform in platforms)
                        {
                            if (platform.IP.Equals(reply.Address) && platform.Port == port)
                            {
                                newPlatform = platform;
                                needAdd = false;
                                break;
                            }
                        }

                        if (needAdd)
                            platforms.Add(newPlatform);

                        if (autofocus)
                            comboBox.SelectedItem = newPlatform;
                    }));
                }
            });
        }


        private void ScanNetworkForCompatibleDevices(IPAddress startAddress)
        {
            byte[] address = startAddress.GetAddressBytes();
            byte originalIndex = address[address.Length-1];
            for (byte i = 0; i < 255; ++i)
            {
                if (i != originalIndex)
                {
                    address[address.Length-1] = i;
                    IPAddress ip = new IPAddress(address);

                    AddPlatform(new IPAddress(address), PlatformDescription.DEFAULT_PORT, false);
                }
            }
        }
*/            

        private void comboBox_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            PlatformDescription desc = comboBox.SelectedItem as PlatformDescription;

            if (desc !=null)
            {
                IP.Text = desc.IP.ToString();
                Port.Text = desc.Port.ToString();
                DetailedIP.Visibility = desc != null && desc.Detailed ? Visibility.Visible : Visibility.Collapsed;
            }
        }

        private void IP_TextChanged(object sender, TextChangedEventArgs e)
        {
            PlatformDescription desc = comboBox.SelectedItem as PlatformDescription;
            if (desc != null)
            {
                IPAddress address = null;
                if (IPAddress.TryParse(IP.Text, out address))
                    desc.IP = address;
            }
        }

        private void Port_TextChanged(object sender, TextChangedEventArgs e)
        {
            PlatformDescription desc = comboBox.SelectedItem as PlatformDescription;
            if (desc != null)
            {
                short port = 0;
                if (short.TryParse(Port.Text, out port))
                    desc.Port = port;
            }
        }

        
    }
}
