using System;
using System.Collections.Generic;
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

        public string MaskString
        {
            get { return String.Format("0x{0:X8}", mask_value); }
        }

        public List<T> GetChildObjects<T>(DependencyObject obj) where T : FrameworkElement
        { 
            DependencyObject child = null;
            List<T> childList = new List<T>();

            for (int i = 0; i <= VisualTreeHelper.GetChildrenCount(obj) - 1; i++)
            {
                child = VisualTreeHelper.GetChild(obj, i);
                if (child is T) // && (((T)child).GetType() == typename))
                {
                    childList.Add((T)child);
                }
                childList.AddRange(GetChildObjects<T>(child));
            }
            return childList;
        }

        private void Button_Check(object sender, RoutedEventArgs e)
        {
            int tag = int.Parse( ((CheckBox)sender).Tag.ToString() );
            mask_value = mask_value | ( 1u << tag);

            maskValueText.Content = String.Format("Mask Value: 0x{0:X8}", mask_value);
        }

        private void Button_UnCheck(object sender, RoutedEventArgs e)
        {
            int tag = int.Parse(((CheckBox)sender).Tag.ToString());
            mask_value = mask_value & ~(1u << tag);
            
            maskValueText.Content = String.Format("Mask Value: 0x{0:X8}", mask_value);
        }

        private void ConfirmButton_Click(object sender, RoutedEventArgs e)
        {
            this.Close();
        }

        private void CheckAllButton_Click(object sender, RoutedEventArgs e)
        {
            List<CheckBox> list = GetChildObjects<CheckBox>(this);
            foreach (CheckBox box in list)
            {
                box.IsChecked = true;
            }
        }

        private void UncheckAllButton_Click(object sender, RoutedEventArgs e)
        {
            List<CheckBox> list = GetChildObjects<CheckBox>(this);
            foreach (CheckBox box in list)
            {
                box.IsChecked = false;
            }
        }

        private void Window_Loaded(object sender, RoutedEventArgs e)
        {
            List<CheckBox> list = GetChildObjects<CheckBox>(this);
            foreach (CheckBox box in list)
            {
                int tag = int.Parse(box.Tag.ToString());
                if ((mask_value & (1u << tag)) == (1u << tag))
                {
                    box.IsChecked = true;
                }
                else
                {
                    box.IsChecked = false;
                }
            }

            filterThreshText.Text = filter_thresh.ToString();
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
    }
}
