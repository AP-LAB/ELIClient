using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Graphics.Imaging;
using Windows.Media;
using Windows.Media.Capture;
using Windows.Media.MediaProperties;
using Windows.Storage.Streams;

using Windows.Media.Capture.Frames;
using Windows.Devices.Enumeration;
using Windows.Media.Capture;
using Windows.UI.Xaml.Media.Imaging;
using Windows.Media.MediaProperties;
using Windows.Graphics.Imaging;
using System.Threading;
using Windows.UI.Core;
using System.Threading.Tasks;
using Windows.Media.Devices;

namespace ELIClient
{
    class Capturer
    {
        public MediaCapture _mediaCapture;

        public bool isSetUp = false;

        OutputVideoStream currentOutputStream;

        public Capturer()
        {
            //SetUpMedkiaCapture();

        }

        public async void SetUpMediaCapture()
        {
            _mediaCapture = new MediaCapture();
            await _mediaCapture.InitializeAsync();

            IRandomAccessStream videostream = new InMemoryRandomAccessStream();

            await _mediaCapture.StartRecordToStreamAsync(MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Vga), videostream);

            isSetUp = true;
        }




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

            //Throw error!

            return foundDevices.First();            
        }

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
            
            //Throw error!

            return foundDevices.First();
        }

        public async void InitializeAndSetMediaCaptureAsync()
        {
            _mediaCapture = new MediaCapture();

            DeviceInformation foundAudioDevice = await GetAudioCaptureDeviceOnMachine();
            DeviceInformation foundVideoDevice = await GetVideoCaptureDeviceOnMachine();          

            var settings = new MediaCaptureInitializationSettings()
            {
                AudioDeviceId = foundAudioDevice.Id,
                VideoDeviceId = foundVideoDevice.Id,
                SharingMode = MediaCaptureSharingMode.ExclusiveControl,
                MemoryPreference = MediaCaptureMemoryPreference.Cpu,
                StreamingCaptureMode = StreamingCaptureMode.AudioAndVideo
            };

            await _mediaCapture.InitializeAsync(settings);
            isSetUp = true;

        }

        public async void StartVideoRecording()
        {
            //Make sure the MediaCapture object is initialized
            await CheckSetUp();

            try
            {
                Streamer streamer = new Streamer("localhost", "4567");
                await streamer.Connect();
                //When the streamer is connected, create a new Output stream using the streamer
                currentOutputStream = new OutputVideoStream(streamer);

                GC.KeepAlive(currentOutputStream);

                //Start recording
                await _mediaCapture.StartRecordToStreamAsync(MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Vga), currentOutputStream);


                //while (true)
                //{
                //    await Task.Delay(TimeSpan.FromSeconds(1));
                //    Debug.WriteLine(_mediaCapture.ThermalStatus);
                //}

            }
            catch (Exception ex)
            {
                Debug.WriteLine(ex.Message);
                Debug.WriteLine(ex.Source);
                //TODO error management
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

        public async void A()
        {
            while(!isSetUp){
                await Task.Delay(TimeSpan.FromSeconds(1));
            }
            var stream = new InMemoryRandomAccessStream();

            await _mediaCapture.StartRecordToStreamAsync(MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Vga), stream);

        }


        //https://docs.microsoft.com/en-us/windows/uwp/audio-video-camera/process-media-frames-with-mediaframereader


    }


}
