using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace ELIClient.Views
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class RandomConnectionView : Page
    {
        public RandomConnectionView()
        {
            this.InitializeComponent();
            //TODO go to next view and get message
            Connect();
        }

        public async void Connect()
        {
            await App.serverConnector.SendRandomConnectionRequestMessageAsync();
            var frame = (Frame)Window.Current.Content;

            dynamic jsonMessage = await App.serverConnector.ReceiveMessage();
            //Start streaming if the "Start recording" message is received.
            if (jsonMessage.GetNamedString("MESSAGE").Equals("Start recording"))
            {
                frame.Navigate(typeof(VideoCallView.VideoCallView), App.serverConnector);
            }
        }

    }
}
