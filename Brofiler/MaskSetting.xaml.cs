using System;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Data;
using System.Windows.Documents;
using System.Windows.Input;
using System.Windows.Media;
using System.Windows.Media.Imaging;
using System.Windows.Shapes;
using System.ComponentModel;
using System.Windows.Threading;
using AurelienRibon.Ui.SyntaxHighlightBox;

namespace Profiler
{
    /// <summary>
    /// Interaction logic for MaskSetting.xaml
    /// </summary>
    public partial class MaskSetting : Window
    {
        private UInt32 mask_value = 0;
        private int filter_thresh = 0;

        public MaskSetting()
        {
            InitializeComponent();
        }

        public UInt32 MaskValue
        {
            get { return mask_value; }
            set { mask_value = value; }
        }

        public int FilterThresh
        {
            get { return filter_thresh; }
            set { filter_thresh = value; }
        }

        public class MaskItem : INotifyPropertyChanged
        {
            private bool _checked = false;
            private string _name = "";
            public int Idx { get; set; }

            public string Name 
            {
                get { return _name; }
                set
                {
                    if (_name != value)
                    {
                        _name = value;
                        NotifyPropertyChanged("Name");
                    }
                }
            }
            public bool Checked 
            {
                get { return _checked; }
                set
                {
                    if (_checked != value)
                    {
                        _checked = value;
                        NotifyPropertyChanged("Checked");
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

        private List<MaskItem> mask_item_list = new List<MaskItem>();

        //public List<T> GetChildObjects<T>(DependencyObject obj) where T : FrameworkElement
        //{ 
        //    DependencyObject child = null;
        //    List<T> childList = new List<T>();

        //    for (int i = 0; i <= VisualTreeHelper.GetChildrenCount(obj) - 1; i++)
        //    {
        //        child = VisualTreeHelper.GetChild(obj, i);
        //        if (child is T) // && (((T)child).GetType() == typename))
        //        {
        //            childList.Add((T)child);
        //        }
        //        childList.AddRange(GetChildObjects<T>(child));
        //    }
        //    return childList;
        //}

        private void UpdateMaskValue()
        {
            mask_value = 0;
            foreach (var item in mask_item_list)
            {
                if( item.Checked )
                    mask_value = mask_value | (1u << item.Idx);
            }
            maskValueText.Content = String.Format("Mask Value: 0x{0:X8}", mask_value);
        }

        private void FormatMaskMacroDefinitions()
        {
            string macro_string = "// Auto-generated macro definitions. You can\r\n// copy-and-paste it to your project header file.\r\n";
            foreach (var item in mask_item_list)
            {
                if (item.Name.Length > 0)
                {
                    macro_string += String.Format("#define BROFILER_MASK_{0}\t\t(1 << {1})\r\n", item.Name.ToUpper(), item.Idx);
                }
            }
            shbox.Text = macro_string;
        }

        private void WriteMacroDefinitions()
        {
            StringCollection collection = new StringCollection();
            foreach (var item in mask_item_list)
            {
                if (item.Name.Length > 0)
                {
                    collection.Add(String.Format("{0}, {1}", item.Idx, item.Name));
                }
            }
            Properties.Settings.Default.MaskDefinitions = collection;
            Properties.Settings.Default.Save();
        }

        private void ConfirmButton_Click(object sender, RoutedEventArgs e)
        {
            WriteMacroDefinitions();
            this.Close();
        }

        private void CheckAllButton_Click(object sender, RoutedEventArgs e)
        {
           foreach (var item in mask_item_list)
            {
                item.Checked = true;
            }
        }

        private void UncheckAllButton_Click(object sender, RoutedEventArgs e)
        {
            foreach (var item in mask_item_list)
            {
                item.Checked = false;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            shbox.CurrentHighlighter = HighlighterManager.Instance.Highlighters["VHDL"];
            for (int i = 0; i < 32; i++)
            {
                var item = new MaskItem() { Idx = i, Name = "", Checked = false };
                if ((mask_value & (1u << i)) == (1u << i))
                    item.Checked = true;
                else
                    item.Checked = false;
                item.PropertyChanged += Mask_Item_PropertyChanged;
                mask_item_list.Add(item);
            }

            var MaskBitSettings = Properties.Settings.Default.MaskDefinitions;
            foreach (var s in MaskBitSettings)
            {
                var bits_info = ((string)(s)).Split(',');
                if (bits_info.Length != 2)
                {
                    Console.WriteLine("[Error] Invalid mask bit definition: {0}", s);
                    continue;
                }

                var bit_idx = 0;
                try
                {
                    bit_idx = Int32.Parse(bits_info[0].Trim());
                }
                catch
                {
                    Console.WriteLine("[Error] Invalid mask bit definition: {0}", s);
                    continue;
                }
                var bit_name = bits_info[1].Trim();

                mask_item_list[bit_idx].Name = bit_name;
            }

            
            lbMaskBits.ItemsSource = mask_item_list;
            UpdateMaskValue();

            FormatMaskMacroDefinitions();

            filterThreshText.Text = filter_thresh.ToString();
        }

        private void Mask_Item_PropertyChanged(object sender, PropertyChangedEventArgs e)
        {
            UpdateMaskValue();
        }

        private void filterThreshText_TextChanged(object sender, TextChangedEventArgs e)
        {
            int value = 0;
            if (int.TryParse(filterThreshText.Text, out value))
            {
                filter_thresh = value;
            }
            else
            {
                filter_thresh = 0;
                filterThreshText.Text = "0";
            }
        }

        private void TextBlock_MouseDown(object sender, MouseButtonEventArgs e)
        {
            if (e.ClickCount == 2)
            {
                //TextBox txt = (TextBox)((StackPanel)((TextBlock)sender).Parent).Children[3];
                TextBox txt = (TextBox)((StackPanel)(sender)).Children[3];
                TextBlock tb = (TextBlock)((StackPanel)(sender)).Children[2];
                tb.Visibility = Visibility.Collapsed;
                txt.Visibility = Visibility.Visible;

                txt.SelectAll();
                this.Dispatcher.BeginInvoke((Action)(() => Keyboard.Focus(txt)), DispatcherPriority.Render);
            }
        }

        private void TextBox_LostFocus(object sender, RoutedEventArgs e)
        {
            TextBlock tb = (TextBlock)((StackPanel)((TextBox)sender).Parent).Children[2];
            tb.Text = ((TextBox)sender).Text;
            tb.Visibility = Visibility.Visible;
            ((TextBox)sender).Visibility = Visibility.Collapsed;

            FormatMaskMacroDefinitions();
        }

        private void TextBox_KeyDown(object sender, KeyEventArgs e)
        {
            if (e.Key.Equals(Key.Enter))
            {
                TextBox_LostFocus(sender, null);
            }
        }
    }
}
