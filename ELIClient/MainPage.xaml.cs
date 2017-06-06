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

            Capturer c = new Capturer();

            //c.SetUpMediaCapture();
            StartPreview(c);

            //c.StartLiveStream();





        }

        async void StartPreview(Capturer capturer) {

            //Make sure the capturer is set up.
            if (!capturer.isSetUp)
            {
                await Task.Delay(TimeSpan.FromSeconds(1));
            }

            //Set of the Frames Per Second and the time for every frame.
            int FPS = 60;
            int framePeriod = 1000 / FPS;
            Debug.WriteLine(framePeriod);
            Stopwatch stopwatch = new Stopwatch();
            while (true)
            {
                //Start timer
                stopwatch.Restart();

                // Create a video frame in the desired format for the preview frame
                var softwareBitmap = await capturer.GetFrame();

                //Convert the photo to a SoftwareBitmap with a pixelformat Bgra8 and a premultiplied alpahmode. 
                //This conversion is needed for creating a SoftwareBitmapSource out of the SoftwareBitmap.
                SoftwareBitmap displayableImage = SoftwareBitmap.Convert(softwareBitmap, BitmapPixelFormat.Bgra8, BitmapAlphaMode.Premultiplied);
                //Create a SoftwareBitmapSource and copy the SoftwareBitmap into it.
                var softwareBitmapSource = new SoftwareBitmapSource();
                await softwareBitmapSource.SetBitmapAsync(displayableImage);
                //Set the source of the image in the xaml form.
                image.Source = softwareBitmapSource;

                //Stop timer
                stopwatch.Stop();               
                int sleeptime = framePeriod - (int) stopwatch.ElapsedMilliseconds;
                //If there is any time left, wait that amount of time.
                if (sleeptime > 0)
                {
                    await Task.Delay(sleeptime);
                }
            }
        }
    }
}
