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

namespace ChromeClientWPF
{
    /// <summary>
    /// Interaction logic for MainWindow.xaml
    /// </summary>
    public partial class MainWindow : Window
    {
        public MainWindow()
        {
            InitializeComponent();

            
        }
        // Can execute
        private void CommandBinding_CanExecute(object sender, CanExecuteRoutedEventArgs e)
        {
            e.CanExecute = true;
        }

        // Minimize
        private void CommandBinding_Executed_Minimize(object sender, ExecutedRoutedEventArgs e)
        {
            SystemCommands.MinimizeWindow(this);
        }

        // Maximize
        private void CommandBinding_Executed_Maximize(object sender, ExecutedRoutedEventArgs e)
        {
            if (WindowState == WindowState.Maximized)
            {
                SystemCommands.RestoreWindow(this);
            }
            else
            {
                SystemCommands.MaximizeWindow(this);
            }
        }

        // Restore
        private void CommandBinding_Executed_Restore(object sender, ExecutedRoutedEventArgs e)
        {
            SystemCommands.RestoreWindow(this);
        }

        // Close
        private void CommandBinding_Executed_Close(object sender, ExecutedRoutedEventArgs e)
        {
            SystemCommands.CloseWindow(this);
        }

        private void Button_Click(object sender, RoutedEventArgs e)
        {

            var dic = Application.Current.Resources.MergedDictionaries[0];

            dic.Remove("ControlGlythColour");

            SolidColorBrush dd = new SolidColorBrush(Colors.Yellow);


            dic.Add("ControlGlythColour", dd);

            //System::Windows::ResourceDictionary ^ dic = Resources->MergedDictionaries[0];
            //System::Windows::Style ^ st = (System::Windows::Style ^)dic["MainWindowStyle"];
            //System::Windows::Media::SolidColorBrush^ bb = gcnew System::Windows::Media::SolidColorBrush(Colors::Yellow);
            //(System::Windows::Media::SolidColorBrush^)dic["ControlGlythColour"] = bb;
            //dic->Remove("ControlGlythColour");
            //dic->Add("ControlGlythColour", bb);
            //this->MainWindow->Style = st;


            //System::Windows::ResourceDictionary ^ dic1 = this->MainWindow->Resources;


        }
    }
}
