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

            plotter.Children.RemoveAll(typeof(LineGraph));
            var lineGraph = plotter.AddLineGraph(dataSource1, Colors.Green, 2, item.name);
            lineGraph.Filters.Add(new InclinationFilter());
            lineGraph.Filters.Add(new FrequencyFilter());
            plotter.FitToView();
            VerticalAxisTitle.Content = item.name;
        }

        private void Combo2_SelectionChanged(object sender, SelectionChangedEventArgs e)
        {
            ComboBox cmb = sender as ComboBox;
            uint id = (uint)(cmb.SelectedValue);
            CounterNameIDPair item = (CounterNameIDPair)(cmb.SelectedItem);

            dataSource2 = new EnumerableDataSource<CounterPoint>(counters.GetCounterArray(id));
            dataSource2.SetXMapping(x => x.Id);
            dataSource2.SetYMapping(y => y.Count);

            plotter2.Children.RemoveAll(typeof(LineGraph));
            var lineGraph = plotter2.AddLineGraph(dataSource2, Colors.Red, 2, item.name);
            lineGraph.Filters.Add(new InclinationFilter());
            lineGraph.Filters.Add(new FrequencyFilter());
            plotter2.FitToView();
            VerticalAxisTitle2.Content = item.name;
        }
    }
}
