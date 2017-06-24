using ELIClient.SimpleELIServerCommunication;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices.WindowsRuntime;
using System.Threading.Tasks;
using Windows.Data.Json;
using Windows.Foundation;
using Windows.Foundation.Collections;
using Windows.Media.Playback;
using Windows.Networking.Sockets;
using Windows.UI.Xaml;
using Windows.UI.Xaml.Controls;
using Windows.UI.Xaml.Controls.Primitives;
using Windows.UI.Xaml.Data;
using Windows.UI.Xaml.Input;
using Windows.UI.Xaml.Media;
using Windows.UI.Xaml.Navigation;

// The Blank Page item template is documented at https://go.microsoft.com/fwlink/?LinkId=234238

namespace ELIClient.VideoCallView
{
    /// <summary>
    /// An empty page that can be used on its own or navigated to within a Frame.
    /// </summary>
    public sealed partial class VideoCallView : Page
    {
        public static MediaPlaybackList _playlist = new MediaPlaybackList();
        private Capturer capturer = new Capturer();
        private InputVideoStream inStream;
        Receiver receiver;

        public VideoCallView()
        {
            this.InitializeComponent();           
        }

        protected override void OnNavigatedTo(NavigationEventArgs e)
        {
            var serverConnector = e.Parameter as ServerConnector;

            SetUpStreams(serverConnector);
        }

        /// <summary>
        /// Await a server response that indicates that a client to connect to is found.
        /// When the correct message ("Start recording") is passed, the capturing and streaming is started.
        /// </summary>
        /// <param name="serverConnector"></param>
        private async void SetUpStreams(ServerConnector serverConnector)
        {
            // Await the server response.
            //dynamic jsonMessage = await serverConnector.ReceiveMessage();
            ////Start streaming if the "Start recording" message is received.
            //if (jsonMessage.MESSAGE.equals("Start recording"))
            //{
            capturer.InitializeAndSetMediaCaptureAsync();
            // capturer.StartVideoRecording(serverConnector.GetSocket());

            Task.Run(() =>
            {
                capturer.StartVideoRecordingOnThread(serverConnector.GetSocket());
            });
            
            

       

                receiver = new Receiver(serverConnector.GetSocket());
                mediaPlayer.MediaEnded += MediaEnded;
                mediaPlayer.MediaFailed += MediaFailed;
                mediaPlayer.MediaOpened += MediaOpened;
                mediaPlayer.SetPlaybackSource(_playlist);

            //inStream = new InputVideoStream(serverConnector.GetSocket());
            //mediaPlayer.SetSource(inStream, "video/HEVC");
            //}
        }


        private void OnExit(object sender, object e)
        {
            Debug.WriteLine("Stopping recording");
            if (capturer.isRecording)
            {
                capturer.StopRecordingAsync();
            }

        }

        private async void SetUpReceiver()
        {
            InputVideoStream receiver = new InputVideoStream(App.serverConnector.GetSocket());
            //await receiver.Connect();
            mediaPlayer.MediaEnded += MediaEnded;
            mediaPlayer.MediaFailed += MediaFailed;
            mediaPlayer.MediaOpened += MediaOpened;
            mediaPlayer.SetPlaybackSource(_playlist);      
        }

        private void MediaOpened(object sender, RoutedEventArgs e)
        {
            Debug.WriteLine("Opened!");
        }

        private void MediaFailed(object sender, ExceptionRoutedEventArgs e)
        {
            Debug.WriteLine("Failed!");
            Debug.WriteLine(e.ErrorMessage);

        }

        private void MediaEnded(object sender, RoutedEventArgs e)
        {
            //Debug.WriteLine("Replay");
            //mediaPlayer.Position = TimeSpan.Zero;
            //mediaPlayer.Play();
        }
        
    }
}
