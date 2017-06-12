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
//using System.Net;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=402352&clcid=0x409

namespace ELIClient
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class MainPage : Page
    {
        public static MediaPlaybackList _playlist = null;
        Capturer capturer = new Capturer();

        public MainPage()
        {
            this.InitializeComponent();




            capturer.InitializeAndSetMediaCaptureAsync();



            //c.A();


            capturer.StartVideoRecording();

            SetUpReceiver();

        }

        private async void SetUpReceiver()
        {
            InputVideoStream receiver = new InputVideoStream("localhost", "4567");
            await receiver.Connect();
            a.SetSource(receiver, "text/plain");
        }

        private void Playlist_ItemFailed(MediaPlaybackList sender, MediaPlaybackItemFailedEventArgs args)
        {
            Debug.WriteLine(args.Error.ErrorCode);
            _playlist.Items.Remove(args.Item);
        }

        
    }
}
