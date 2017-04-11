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
using System.Windows.Navigation;
using System.Windows.Shapes;
using Profiler.Data;
using Microsoft.Research.DynamicDataDisplay.DataSources;
using Microsoft.Research.DynamicDataDisplay;
using Microsoft.Research.DynamicDataDisplay.Filters;
using Microsoft.Research.DynamicDataDisplay.Charts.Navigation;
using System.Collections.ObjectModel;
using System.ComponentModel;


namespace Profiler
{
    /// <summary>
    /// Interaction logic for CounterView.xaml
    /// </summary>
    public partial class CounterView : UserControl
    {
        private CounterCollection counters = new CounterCollection();
        private EnumerableDataSource<CounterPoint> dataSource1 = null;
        private EnumerableDataSource<CounterPoint> dataSource2 = null;

        private LineGraph line1 = null;
        private LineGraph line2 = null;

        public CounterView()
        {
            InitializeComponent();

            Combo1.ItemsSource = counters.CounterNames;
            Combo2.ItemsSource = counters.CounterNames;
            Combo1.DisplayMemberPath = "name";
            Combo1.SelectedValuePath = "id";
            Combo2.DisplayMemberPath = "name";
            Combo2.SelectedValuePath = "id";
        }


        public void ApplyResponse(DataResponse response)
        {
            switch (response.ResponseType)
            {
                case DataResponse.Type.ReportCounters:
                    {
                        counters.Read(response);

                        if(dataSource1 != null)
                            dataSource1.RaiseDataChanged();
                        if (dataSource2 != null)
                            dataSource2.RaiseDataChanged();
                    }
                    break;
            }
        }

        private void Combo1_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cmb = sender as ComboBox;
            uint id = (uint)(cmb.SelectedValue);
            CounterNameIDPair item = (CounterNameIDPair)(cmb.SelectedItem);

            dataSource1 = new EnumerableDataSource<CounterPoint>(counters.GetCounterArray(id));
            dataSource1.SetXMapping(x => x.Id);
            dataSource1.SetYMapping(y => y.Count);
            if (line1 == null)
                line1 = plotter1.AddLineGraph(dataSource1, Colors.Green, 2, item.name);
            else
                line1.DataSource = dataSource1;
            line1.Filters.Add(new InclinationFilter());
            line1.Filters.Add(new FrequencyFilter());

            plotter1.LegendVisible = false;
            plotter1.FitToView();
            VerticalAxisTitle1.Content = item.name;
        }

        private void Combo2_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cmb = sender as ComboBox;
            uint id = (uint)(cmb.SelectedValue);
            CounterNameIDPair item = (CounterNameIDPair)(cmb.SelectedItem);

            dataSource2 = new EnumerableDataSource<CounterPoint>(counters.GetCounterArray(id));
            dataSource2.SetXMapping(x => x.Id);
            dataSource2.SetYMapping(y => y.Count);
            if (line2 == null)
                line2 = plotter2.AddLineGraph(dataSource2, Colors.Red, 2, item.name);
            else
                line2.DataSource = dataSource2;

            line2.Filters.Add(new InclinationFilter());
            line2.Filters.Add(new FrequencyFilter());

            plotter2.LegendVisible = false;
            plotter2.FitToView();
            VerticalAxisTitle2.Content = item.name;
        }

        private void ShowCursor1_Click(object sender, RoutedEventArgs e)
        {
            bool clicked = ShowCursor1.IsChecked ?? false;
            if (clicked)
                plotter1.Children.Add(new CursorCoordinateGraph());
            else
                plotter1.Children.RemoveAll(typeof(CursorCoordinateGraph));
        }

        private void ShowCursor2_Click(object sender, RoutedEventArgs e)
        {
            bool clicked = ShowCursor1.IsChecked ?? false;
            if (clicked)
                plotter2.Children.Add(new CursorCoordinateGraph());
            else
                plotter2.Children.RemoveAll(typeof(CursorCoordinateGraph));
        }
    }
}
