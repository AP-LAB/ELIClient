using System;
using System.Diagnostics;
using System.Linq;
using System.Threading.Tasks;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;
using Windows.Storage.Streams;
using Windows.Devices.Enumeration;
using Windows.Storage;
using Windows.Networking.Sockets;

namespace ELIClient
{
    /// <summary>
    /// This class handles the capturing of video and audio using the MediaCapture class.
    /// After the InitializeAndSetMediaCaptureAsync() method is finished, the recording can be started using the StartVideoRecording() method. 
    /// </summary>
    class Capturer
    {
        public MediaCapture _mediaCapture; //!< The MediaCapture class that is used for capturing.
        public bool isSetUp = false; //!< A boolean that indicates that the InitializeAndSetMediaCaptureAsync() method succeeded.
        public bool isRecording = false; //!< A boolean that indicates that a recording is started and not stopped.
        private OutputVideoStream currentOutputStream; //!< The OutputVideoStream to stream the captured data to.

        /// <summary>
        /// Find a video capture device that is usable by the application.
        /// 
        /// Note: When no device is found, an exception will be thrown.
        /// </summary>
        /// <returns>A video capture device information object.</returns>
        public async Task<DeviceInformation> GetVideoCaptureDeviceOnMachine()
        {
            var foundDevices = await Device​Information.FindAllAsync(DeviceClass.VideoCapture);
            foreach (DeviceInformation deviceInfo in foundDevices)
            {
                if (deviceInfo.IsEnabled)
                {
                    return deviceInfo;
                }
            }

            //TODO Throw error if no  devices are found!
            return foundDevices.First();            
        }

        /// <summary>
        /// Find a audio capture device that is usable by the application.
        /// 
        /// Note: When no device is found, an exception will be thrown.
        /// </summary>
        /// <returns>A audio capture device information object.</returns>
        public async Task<DeviceInformation> GetAudioCaptureDeviceOnMachine()
        {
            var foundDevices = await Device​Information.FindAllAsync(DeviceClass.AudioCapture);
            foreach(DeviceInformation deviceInfo in foundDevices)
            {
                if (deviceInfo.IsEnabled)
                {
                    return deviceInfo;
                }
            }

            //TODO Throw error if no  devices are found!
            return foundDevices.First();
        }

        /// <summary>
        /// This method searches for audio and video devices on the machine.
        /// After the devices are found, settings for the MediaCapture object are created and used to initialize the MediaCapture object.
        /// When finished, the isSetUp boolean is set to true.
        /// The method must be called before the StartRecording method.
        /// </summary>
        public async void InitializeAndSetMediaCaptureAsync()
        {
            _mediaCapture = new MediaCapture();

            DeviceInformation foundAudioDevice = await GetAudioCaptureDeviceOnMachine();
            DeviceInformation foundVideoDevice = await GetVideoCaptureDeviceOnMachine();          

            var settings = new MediaCaptureInitializationSettings()
            {
                AudioDeviceId = foundAudioDevice.Id,
                VideoDeviceId = foundVideoDevice.Id,
                SharingMode = MediaCaptureSharingMode.SharedReadOnly,
                MemoryPreference = MediaCaptureMemoryPreference.Cpu,
                StreamingCaptureMode = StreamingCaptureMode.AudioAndVideo                
            };

            await _mediaCapture.InitializeAsync(settings);
            isSetUp = true;
            _mediaCapture.Failed += Failed;
            _mediaCapture.ThermalStatusChanged += ThermalStatusChanged;
        }

        private void ThermalStatusChanged(MediaCapture sender, object args)
        {
            Debug.WriteLine("Changed: " + sender.ThermalStatus);
        }

        private void Failed(MediaCapture sender, MediaCaptureFailedEventArgs errorEventArgs)
        {
            Debug.WriteLine("Error: " + errorEventArgs.Message);
        }

        /// <summary>
        /// Start video recording to the passed streamsocket.
        /// Please ensure the streamsocket is connected.
        /// </summary>
        /// <param name="_socket">The video socket to stream to.</param>
        public async void StartVideoRecording(StreamSocket _socket)
        {
            //Make sure the MediaCapture object is initialized
            await CheckSetUp();
            try
            {
                Streamer streamer = new Streamer(_socket);
                // When the streamer is connected, create a new Output stream using the streamer
                currentOutputStream = new OutputVideoStream(streamer);
                // Start recording
                //await _mediaCapture.StartRecordToStreamAsync(MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Auto), currentOutputStream);
                await _mediaCapture.StartRecordToStreamAsync(MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Vga), currentOutputStream);
                isRecording = true;     
            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                Debug.WriteLine(ex.Source);
                //TODO error management
            }            
        }

        public async void StartVideoRecordingOnThread(StreamSocket _socket)
        {
            //Make sure the MediaCapture object is initialized
            await CheckSetUp();
            Streamer streamer = new Streamer(_socket);
            // When the streamer is connected, create a new Output stream using the streamer
            isRecording = true;
            while (true)
            {
                InMemoryRandomAccessStream stream = new InMemoryRandomAccessStream();
                await _mediaCapture.StartRecordToStreamAsync(MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Vga), stream);
                await Task.Delay(TimeSpan.FromSeconds(1));
                await _mediaCapture.StopRecordAsync();

                stream.Seek(0);

                Windows.Storage.Streams.Buffer buffer = new Windows.Storage.Streams.Buffer((uint)stream.Size);
                await stream.ReadAsync(buffer, (uint)stream.Size, Windows.Storage.Streams.InputStreamOptions.None);
                streamer.WriteToSocketUsingReader(buffer);
            }          
        }

        public async void StopRecordingAsync()
        {
            if (isRecording)
            {
                await _mediaCapture.StopRecordAsync();
                isRecording = false;
            }
        }

        private async Task CheckSetUp()
        {
            //TODO what if it fails?
            while (!isSetUp)
            {
                await Task.Delay(TimeSpan.FromSeconds(1));
            }
        }
    }

}
