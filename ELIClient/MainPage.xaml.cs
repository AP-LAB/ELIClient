using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Graphics.Imaging;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Media.Imaging;
using Windows.UI.Xaml.Navigation;
using System.Threading;
using System.Threading.Tasks;
using System.Diagnostics;
using Windows.Media.MediaProperties;
using Windows.Media.Capture;
using Windows.Media;
using Windows.Media.Playback;
using Windows.Media.Core;
using Windows.Storage.Streams;
using Windows.Media.Devices;
using System.Net;
using System.Net.Sockets;
using Windows.Networking.Sockets;
using Windows.Networking;
using Windows.ApplicationModel.Core;
using Windows.Data.Json;
using ELIClient.SimpleELIServerCommunication;
using ELIClient.Views;
using Windows.UI.ViewManagement;
//using System.Net;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace ELIClient
{

    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
 
        public MainPage()
        {
            this.InitializeComponent();
            ApplicationView.PreferredLaunchViewSize = new Size(1000, 1300);
            ApplicationView.PreferredLaunchWindowingMode = ApplicationViewWindowingMode.PreferredLaunchViewSize;
        }

        private void RandomConnectionBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(RandomConnectionView));
        }

        private void CityConnectionBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(CityConnectionView));
        }

        private void CodeConnectionBtn_Click(object sender, RoutedEventArgs e)
        {
            this.Frame.Navigate(typeof(UserConnectionView));
        }
    }
}
