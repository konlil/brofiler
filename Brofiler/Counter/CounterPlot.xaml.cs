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


namespace Profiler
{
    /// <summary>
    /// Interaction logic for CounterPlot.xaml
    /// </summary>
    public partial class CounterPlot : UserControl
    {
        public CounterPointArray counterPoints;
        public CounterPlot()
        {
            InitializeComponent();

            counterPoints = new CounterPointArray();
            var ds = new EnumerableDataSource<CounterPoint>(counterPoints);
            plotter.AddLineGraph(ds, Colors.Green);
        }
    }
}
