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


namespace ELIClient
{
    class Capturer
    {
        public MediaCapture _mediaCapture;


        public bool isSetUp = false;
        public Capturer()
        {
            SetUpMediaCapture();

        }

        public async void SetUpMediaCapture()
        {
            _mediaCapture = new MediaCapture();
            await _mediaCapture.InitializeAsync();

            IRandomAccessStream videostream = new InMemoryRandomAccessStream();

            await _mediaCapture.StartRecordToStreamAsync(MediaEncodingProfile.CreateMp4(VideoEncodingQuality.Vga), videostream);

            isSetUp = true;
        }

        public async Task<SoftwareBitmap> TakePhoto()
        {
            Stopwatch s = new Stopwatch();
            s.Start();
            // Prepare and capture photo
            var lowLagCapture = await _mediaCapture.PrepareLowLagPhotoCaptureAsync(ImageEncodingProperties.CreateUncompressed(MediaPixelFormat.Bgra8));
            //Take photo
            var capturedPhoto = await lowLagCapture.CaptureAsync();
            //Get a SoftwareBitmap
            var softwareBitmap = capturedPhoto.Frame.SoftwareBitmap;
            //Release LowLagPhotoCapture object
            await lowLagCapture.FinishAsync();
            s.Stop();
            Debug.WriteLine(s.ElapsedMilliseconds);
            return softwareBitmap;
        }

        public async Task<SoftwareBitmap> GetFrame()
        {
            // Get information about the preview
            var previewProperties = _mediaCapture.VideoDeviceController.GetMediaStreamProperties(MediaStreamType.VideoPreview) as VideoEncodingProperties;

            //Create a video frame in the desired format for the preview frame

            VideoFrame videoFrame = new VideoFrame(BitmapPixelFormat.Bgra8, (int)previewProperties.Width, (int)previewProperties.Height);

            VideoFrame previewFrame = await _mediaCapture.GetPreviewFrameAsync(videoFrame);

            return previewFrame.SoftwareBitmap;

            //http://www.codepool.biz/csharp-camera-api-video-frame.html ?
        }

    }
}
