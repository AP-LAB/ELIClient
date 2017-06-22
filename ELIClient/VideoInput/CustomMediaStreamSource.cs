using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using Windows.Media.Core;
using Windows.Media.MediaProperties;
using Windows.UI.Xaml;

namespace ELIClient
{
    class CustomMediaStreamSource
    {
        private VideoStreamDescriptor _videoDesc = null;
        private const int c_frameRateN = 30;
        private const int c_frameRateD = 1;


        public CustomMediaStreamSource()
        {
            int iWidth = (int)Window.Current.Bounds.Width;
            int iHeight = (int)Window.Current.Bounds.Height;

            // Even frame size with a 16:9 ratio
            iWidth = Math.Min(iWidth, ((iHeight * 16 / 9) >> 1) * 2);
            iHeight = Math.Min(iHeight, ((iWidth * 9 / 16) >> 1) * 2);

            VideoEncodingProperties videoProperties = VideoEncodingProperties.CreateUncompressed(MediaEncodingSubtypes.Bgra8, (uint)iWidth, (uint)iHeight);
            _videoDesc = new VideoStreamDescriptor(videoProperties);
            _videoDesc.EncodingProperties.FrameRate.Numerator = c_frameRateN;
            _videoDesc.EncodingProperties.FrameRate.Denominator = c_frameRateD;
            _videoDesc.EncodingProperties.Bitrate = (uint)(c_frameRateN * c_frameRateD * iWidth * iHeight * 4);

            MediaStreamSource source = new MediaStreamSource(_videoDesc);

            TimeSpan spanBuffer = new TimeSpan(0, 0, 0, 0, 250);
            source.BufferTime = spanBuffer;
            source.Starting += Starting;
            source.SampleRequested += SampleRequested;

        }

        private void SampleRequested(MediaStreamSource sender, MediaStreamSourceSampleRequestedEventArgs args)
        {
            
        }

        private void Starting(MediaStreamSource sender, MediaStreamSourceStartingEventArgs args)
        {
            throw new NotImplementedException();
        }
    }
}
